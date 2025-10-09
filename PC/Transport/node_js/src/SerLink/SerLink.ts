import { SerialPort } from "serialport";
import { ReadlineParser } from "@serialport/parser-readline";
import * as readline from "readline";

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

class Port extends DebugPrint {
  private port: SerialPort;
  private parser: ReadlineParser;
  private onOpen: (() => void) | null = null;
  private onReceive: ((line: string) => void) | null = null;

  constructor(portName: string, baudRate: number, debugOn: boolean = false,
    debugId: string = 'PRT'){

    super(debugOn, debugId);
    this.port = new SerialPort({
      path: portName,
      baudRate: baudRate
    });
    this.parser = this.port.pipe(new ReadlineParser({ delimiter: "\n" }));
  }

  public setOnOpen(callback: () => void) {
    this.onOpen = callback; 
    this.port.on("open", () => {
      if (this.onOpen) {
        this.onOpen();
      }
    });
  }
  
  public setOnReceive(callback: (line: string) => void) {
    this.onReceive = callback;
    this.parser.on("data", (line: string) => {
      if (this.onReceive) {
        this.onReceive(line);
      }
    });
  }

  // Listen for incoming data
// parser.on("data", (line: string) => {
//   console.log("📥 Received:", line);
// });
}  

export class Writer extends DebugPrint {
  private port: Port;

  constructor(port:Port, debugOn: boolean = false, debugId: string = 'WTR'){
    super(debugOn, debugId);
    this.port = port;
  }
}

export class Reader extends DebugPrint {
  private port: Port
  private writer: Writer;

  constructor(port: Port, writer: Writer, debugOn: boolean = false, debugId: string = 'RDR'){    
    super(debugOn, debugId);
    this.port = port;
    this.writer = writer;
    this.port.setOnReceive(this.onReceiveFrame.bind(this));
  }

  public onReceiveFrame(frame: string):void {
    this.dprint(`Received frame: ${frame}`);
  }
}

export class SerLink extends DebugPrint {
  private port: Port;
  private reader: Reader;
  private writer: Writer;

  constructor(portName: string, baudRate: number,
    debugOn: boolean = false, debugId: string = 'SLK'){

    super(debugOn, debugId);
    this.port = new Port(portName, baudRate, this.debugOn); 
    this.writer = new Writer(this.port, this.debugOn);
    this.reader = new Reader(this.port, this.writer, this.debugOn);
    

    this.port.setOnOpen(() => {
      this.dprint(`Serial port ${portName} opened at ${baudRate} baud.`);
    });

    // Here you would set up the data listener and call reader.onReceiveFrame
    // For example:
    // this.port.onData((data: string) => {
    //   this.reader.onReceiveFrame(data);
    // });
  }
}

