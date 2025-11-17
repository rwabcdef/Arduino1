import { SerialPort } from "serialport";
import { ReadlineParser } from "@serialport/parser-readline";
import * as readline from "readline";

function delay(ms: number): Promise<void> {
  return new Promise(resolve => setTimeout(resolve, ms));
}

class DebugPrint {
  protected debugOn: boolean = false;
  protected debugId: string = '-';

  constructor(debugOn: boolean, debugId: string) {
    this.debugOn = debugOn;
    this.debugId = debugId;
  }
  public dprint(message: string) {
    if (this.debugOn) {
      console.log(`[${this.debugId}]: ${message}`);
    }
  }
}

class Frame {
  static TYPE_TRANSMISSION = 'T';
  static TYPE_UNIDIRECTION = 'U';
  static TYPE_ACK = 'A';

  static LEN_PROTOCOL = 5;
  static LEN_TYPE = 1;
  static LEN_ROLLCODE = 3;
  static LEN_DATALEN = 3;

  static LEN_HEADER = Frame.LEN_PROTOCOL + Frame.LEN_TYPE + Frame.LEN_ROLLCODE + Frame.LEN_DATALEN;
  static LEN_ACK = Frame.LEN_PROTOCOL + Frame.LEN_TYPE + Frame.LEN_ROLLCODE;

  static ACK_OK = 900;

  private protocol: string;
  private type: string;
  private rollCode: number;
  private dataLen: number;
  private data: string | undefined;

  public static maxRollCode() {
    let value = 1;
    for (let i = 0; i < Frame.LEN_ROLLCODE; i++) {
      value = value * 10;
    }
    return value - 1;
  }

  constructor(protocol: string, type: string, rollCode: number = 0, dataLen: number = 0, data: string | undefined = undefined) {
    this.protocol = protocol;
    this.type = type;
    this.rollCode = rollCode;
    this.dataLen = dataLen;
    this.data = data;
  }

  public getProtocol(): string { return this.protocol; }
  public getType(): string { return this.type; }
  public getRollCode(): number { return this.rollCode; }
  public getDataLen(): number { return this.dataLen; }
  public getData(): string | undefined { return this.data; }

  // ✅ Equivalent of Python's fromString()
  public static fromString(str: string): Frame {
    let start = 0;
    let end = Frame.LEN_PROTOCOL;
    const protocol = str.substring(start, end);

    start = end;
    end += Frame.LEN_TYPE;
    const type = str.substring(start, end);

    start = end;
    end += Frame.LEN_ROLLCODE;
    const rollCode = parseInt(str.substring(start, end));

    start = end;
    end += Frame.LEN_DATALEN;
    const dataLen = parseInt(str.substring(start, end));

    start = end;
    end = str.length;
    const data = str.substring(start, end);

    const frame = new Frame(protocol, type, rollCode, dataLen, data)
    return frame;
  }

  // ✅ Equivalent of Python's toString()
  public toString(): string {
    const rollCodeStr = String(this.rollCode).padStart(Frame.LEN_ROLLCODE, '0');
    const dataLenStr = String(this.dataLen).padStart(Frame.LEN_DATALEN, '0');
    if (this.data == undefined || this.data?.length == 0) {
      return `${this.protocol}${this.type}${rollCodeStr}${dataLenStr}\n`;
    } else {
      return `${this.protocol}${this.type}${rollCodeStr}${dataLenStr}${this.data}\n`;
    }
  }
}

class Port extends DebugPrint {
  private port: SerialPort | null = null;
  private parser: ReadlineParser | null = null;
  private onOpen: (() => void) | null = null;
  private onReceive: ((line: string) => void) | null = null;
  private busy: boolean = false;

  constructor(debugOn: boolean = false, debugId: string = 'PRT'){
    super(debugOn, debugId);
  }

  public init(portName: string, baudRate: number): void {
    this.port = new SerialPort({
      path: portName,
      baudRate: baudRate
    });
    this.parser = this.port.pipe(new ReadlineParser({ delimiter: "\n" }));
  }

  public setOnOpen(callback: () => void) {
    this.onOpen = callback; 
    this.port?.on("open", () => {
      if (this.onOpen) {
        this.onOpen();
      }
    });
  }
  
  public setOnReceive(callback: (line: string) => void) {
    this.onReceive = callback;
    this.parser?.on("data", (line: string) => {
      if (this.onReceive) {
        this.onReceive(line);
      }
    });
  }

  public write(data: string): void {
    const line = data.endsWith('\n') ? data : data + '\n';
    if(this.port){
      if(this.busy){
        console.log('Port wrie is busy')
      } else {
        this.busy = true;
        this.port.write(line, (err) => {
          if (err) {
            console.error('Error writing to port:', err.message);
            this.busy = false;
            return;
          }
          console.log('Message written successfully: ' + line);
          this.busy = false;
        });
      }
    }
  }

  public async writeAsync(data: string, retryDelay: number = 50): Promise<void> {
    const line = data.endsWith('\n') ? data : data + '\n';

    while (this.busy) {
      await delay(retryDelay); // wait until port is free
    }

    return new Promise((resolve, reject) => {
      this.busy = true;
      this.port?.write(line, (err) => {
        this.busy = false;
        if (err) {
          console.error('Error writing to port:', err.message);
          reject(err);
          return;
        }
        this.dprint(`Message written successfully: ${line.trim()}`);
        resolve();
      });
    });
  }

  public isBusy(): boolean { return this.busy; }

  // Listen for incoming data
// parser.on("data", (line: string) => {
//   console.log("📥 Received:", line);
// });
}  

export class Writer extends DebugPrint {
  private port: Port | null = null;
  private ackFrame: Frame | null = null;

  constructor(debugOn: boolean = false, debugId: string = 'WTR'){
    super(debugOn, debugId);
  }

  public init(port:Port): void {
    this.port = port;
  }

  public async sendFrameAsync(txFrame: Frame):Promise<number> {
    this.dprint(`Sending frame: ${txFrame.toString().trim()}`);
    if (this.port) {
      await this.port.writeAsync(txFrame.toString());

      // Wait for ACK if frame type is TRANSMISSION
      if (txFrame.getType() == Frame.TYPE_TRANSMISSION) {
        this.dprint(`Waiting for ACK for roll code: ${txFrame.getRollCode()}`);
        // Simple wait loop for ACK - in real implementation, consider timeout and retries
        var count = 0;
        while (this.ackFrame == null || this.ackFrame.getRollCode() != txFrame.getRollCode()) {
          await delay(10); // wait for ACK
          count += 1;
          if (count > 100) { // timeout after 1 second
            this.dprint(`Timeout waiting for ACK for roll code: ${txFrame.getRollCode()}`);
            return 1; // timeout error code
          }
        }
        this.dprint(`Received ACK for roll code: ${txFrame.getRollCode()}`);
        this.ackFrame = null; // reset for next frame
        return 0; // success
      }
      else if (txFrame.getType() == Frame.TYPE_UNIDIRECTION) {
        // No ACK expected for UNIDIRECTION frames
        return 0; // success
      }
      else {
        this.dprint(`Unknown frame type: ${txFrame.getType()}`);
        return 2; // unknown frame type error code
      }
    }
    return 3; // port not initialized error code
  }

  public onReceiveAck(ackFrame: Frame): void {
    this.dprint(`ACK frame received: ${ackFrame.toString().trim()}`);
    this.ackFrame = ackFrame;
  }
}

export class Reader extends DebugPrint {
  private port: Port | null = null;
  private writer: Writer | null = null;
  private parent: SerLink | null = null;

  constructor(debugOn: boolean = false, debugId: string = 'RDR'){    
    super(debugOn, debugId);
  }

  public init(port:Port, writer: Writer, parent: SerLink): void {
    this.port = port;
    this.writer = writer;
    this.parent = parent;
    this.port.setOnReceive(this.onReceiveFrame.bind(this));
  }

  public async onReceiveFrame(line: string):Promise<void> {
    this.dprint(`Received line: ${line}`);
    const rxFrame: Frame = Frame.fromString(line)

    if(rxFrame.getType() == Frame.TYPE_TRANSMISSION){
      // A frame of type TRANSMISSION received - send ACK

      const ackFrame = new Frame(rxFrame.getProtocol(), Frame.TYPE_ACK, rxFrame.getRollCode(), Frame.ACK_OK);

      // to do - write ack frame
      if (this.port) {
        await this.port.writeAsync(ackFrame.toString());
        this.dprint(`ACK frame sent: ${ackFrame.toString().trim()}`);
      }
      // Notify parent SerLink
      if (this.parent) {
        this.parent.onReceiveFrame(rxFrame);
      }
    }
    else if(rxFrame.getType() == Frame.TYPE_UNIDIRECTION){
      // A frame of type UNIDIRECTION received - notify parent SerLink
      if (this.parent) {
        this.parent.onReceiveFrame(rxFrame);
      } 
    }
    else if(rxFrame.getType() == Frame.TYPE_ACK){
      // A frame of type ACK received - notify Writer
      if (this.writer) {
        this.writer.onReceiveAck(rxFrame);
      }
    }
    else{
      this.dprint(`Unknown frame type received: ${rxFrame.getType()}`);
    }
  }
}

export class SerLink extends DebugPrint {
  private port: Port | null = null;
  private reader: Reader | null = null;
  private writer: Writer | null = null;

  constructor(debugOn: boolean = false, debugId: string = 'SLK'){

    super(debugOn, debugId);
    
    

    

    // Here you would set up the data listener and call reader.onReceiveFrame
    // For example:
    // this.port.onData((data: string) => {
    //   this.reader.onReceiveFrame(data);
    // });
  }

  public init(portName: string, baudRate: number): void {
    this.port = new Port(this.debugOn);
    this.port.init(portName, baudRate);
    this.writer = new Writer(this.debugOn);
    this.writer.init(this.port);
    this.reader = new Reader(this.debugOn);
    this.reader.init(this.port, this.writer, this)

    this.port.setOnOpen(() => {
      this.dprint(`Serial port ${portName} opened at ${baudRate} baud.`);
    });
  }

  public onReceiveFrame(rxFrame: Frame): void {
    this.dprint(`SerLink received frame: ${rxFrame.toString().trim()}`);
    // Handle the received frame as needed
  }
}

