import { Socket, RxDataHandler } from "../../SerLink/SerLink";

export class Led {
  protected id: string;
  protected socket: Socket | null = null;
  protected onRxData: RxDataHandler | null = null;

  constructor(id: string, socket: Socket | null = null, rxHandler: RxDataHandler | null = null){
    this.id = id;
    this.socket = socket;
    this.onRxData = rxHandler;
  }

  public async handleRxData(data: string):Promise<void> {
    if(this.onRxData){
      this.onRxData(data);
    }
  }
  // Turn LED on
  public async on():Promise<number> {
    if(!this.socket){
      return -1; // error - socket not initialized
    }
    const payload = `${this.id}1`;
    return (await this.socket.sendData(payload, true)).status;
  }

  // Turn LED off
  public async off():Promise<number> {
    if(!this.socket){
      return -1; // error - socket not initialized
    }
    const payload = `${this.id}0`;
    return (await this.socket.sendData(payload, true)).status;
  }

  // Flash LED
  public async flash(numFlashes: number, onPeriods: number, offPeriods: number, finalFlashOff: boolean):Promise<number> {
    if(!this.socket){
      return -1; // error - socket not initialized
    }
    const pad2 = (n: number) => n.toString().padStart(2, "0");
    //const payload = `${this.id}f,${times},${onPeriod},${offPeriod}`;
    const payload =
      `${this.id}F` +
      pad2(numFlashes) +
      pad2(onPeriods) +
      pad2(offPeriods)
      + (finalFlashOff ? '1' : '0');
    return (await this.socket.sendData(payload, true)).status;
  }

  // Enable flash end event
  public async enableFlashEndEvent():Promise<number> {
    if(!this.socket){
      return -1; // error - socket not initialized
    }
    const payload = `${this.id}S`;
    return (await this.socket.sendData(payload, true)).status;
  }

  // Cancel flash end event
  public async cancelFlashEndEvent():Promise<number> {
    if(!this.socket){
      return -1; // error - socket not initialized
    }
    const payload = `${this.id}C`;
    return (await this.socket.sendData(payload, true)).status;
  }
}

export class Motor {
  static MOTOREVENT__PERCENT = 'P';
  static MOTOREVENT__DIRECTION = 'D';
  static MOTOREVENT__FREQUENCY = 'F';

  static MOTOREVENT__DIRECTION_FORWARD = 'F';
  static MOTOREVENT__DIRECTION_REVERSE = 'R';
  static MOTOREVENT__DIRECTION_DISABLED = 'D';

  static MOTOREVENT__FREQUENCY_500_HZ = '0';
  static MOTOREVENT__FREQUENCY_1_KHZ = '1';
  static MOTOREVENT__FREQUENCY_2_KHZ = '2';
  static MOTOREVENT__FREQUENCY_5_KHZ = '3';
  static MOTOREVENT__FREQUENCY_10_KHZ = '4';
  static MOTOREVENT__FREQUENCY_20_KHZ = '5';

  static MOTOREVENT__GET = 'G';

  protected id: string;
  protected socket: Socket | null = null;

  constructor(id: string, socket: Socket | null = null){
    this.id = id;
    this.socket = socket;
  }

  public async setPercent(value: number):Promise<number> {
    if(!this.socket){
      return -1; // error - socket not initialized
    }

    if(value < 0 || value > 100){
      return -2; // error - value out of range
    }

    const absValue = Math.abs(value).toString().padStart(3, '0');
    const payload = `${this.id}${Motor.MOTOREVENT__PERCENT}${absValue}`;
    return (await this.socket.sendData(payload, true)).status;
  }

  public async setDirection(direction: string):Promise<number> {
    if(!this.socket){
      return -1; // error - socket not initialized
    }

    const payload = `${this.id}${Motor.MOTOREVENT__DIRECTION}${direction[0].toUpperCase()}`;
    return (await this.socket.sendData(payload, true)).status;
  }

  public async setFrequency(frequency: string):Promise<number> {
    if(!this.socket){
      return -1; // error - socket not initialized
    }

    const payload = `${this.id}${Motor.MOTOREVENT__FREQUENCY}${frequency[0].toUpperCase()}`;
    return (await this.socket.sendData(payload, true)).status;
  }

  public async getPercent():Promise<number> {
    if(!this.socket){
      return -1; // error - socket not initialized
    }
    
    const payload = `${this.id}${Motor.MOTOREVENT__GET}${Motor.MOTOREVENT__PERCENT}`;
    const result = await this.socket.sendData(payload, true);
    if(result.status == 0 && result.ackData){
      return parseInt(result.ackData);
    }
    return -2; // error - failed to get percent
  }
  
  public async getDirection():Promise<string> {
    if(!this.socket){
      return ''; // error - socket not initialized
    }
    const payload = `${this.id}${Motor.MOTOREVENT__GET}${Motor.MOTOREVENT__DIRECTION}`;
    const result = await this.socket.sendData(payload, true);
    if(result.status == 0 && result.ackData){
      return result.ackData;
    }
    return ''; // error - failed to get direction
  }

  public async getFrequency():Promise<string> {
    if(!this.socket){
      return ''; // error - socket not initialized
    }
    const payload = `${this.id}${Motor.MOTOREVENT__GET}${Motor.MOTOREVENT__FREQUENCY}`;
    const result = await this.socket.sendData(payload, true);
    if(result.status == 0 && result.ackData){
      return result.ackData;
    }
    return ''; // error - failed to get frequency
  }
} 