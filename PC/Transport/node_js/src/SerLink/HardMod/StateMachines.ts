import { SerLink, CLI, Frame, Socket, RxDataHandler, StateFunction, StateMachine } from "../../SerLink/SerLink";
import { Led } from "./Std";
import { config } from "./config";

//const PERIOD_mS = 250; // base period for timing (250ms)

class GreenStateFunction extends StateFunction {
  public pressed: boolean = false;
  public active: boolean = false;
  public timeout: NodeJS.Timeout | null = null;
};

class RedStateFunction extends StateFunction {
  public timeout: NodeJS.Timeout | null = null;
};

export class TrafficLight extends StateMachine {

  static greenLedId = 'G';
  static redLedId = 'R';
  static yellowLedId = 'Y';
  static GREEN = "green";
  static YELLOW = "yellow";
  static RED = "red";

  protected ledRed: Led | null = null;
  protected ledYellow: Led | null = null;
  protected ledGreen: Led | null = null;

  constructor(){
    super();
  }

  public async begin(){
    if(this.ledYellow){
      await this.ledYellow.enableFlashEndEvent(); // Enable flash end event for yellow LED
    }
   this.start(TrafficLight.GREEN);
  }

  public init(ledRed: Led, ledYellow: Led, ledGreen: Led){
    this.ledRed = ledRed;
    this.ledYellow = ledYellow;
    this.ledGreen = ledGreen;
  }
}

export class TrafficLightComponents {
  public ledRed: Led;
  public ledYellow: Led;
  public ledGreen: Led;
  public trafficLight: TrafficLight;

  constructor(ledRed: Led, ledYellow: Led, ledGreen: Led, trafficLight: TrafficLight){
    this.ledRed = ledRed;
    this.ledYellow = ledYellow;
    this.ledGreen = ledGreen;
    this.trafficLight = trafficLight;
  }
}

export interface ITrafficLightParams {
  socket: Socket;
  greenDuration_mS: number;
  redDuration_mS: number;
  numFlashes: number;
  flashOn_mS: number;
  flashOff_mS: number;
}

export class TrafficLightFactory {

  public static create(params: ITrafficLightParams): TrafficLightComponents {

    const trafficLight = new TrafficLight();  // ledRed, ledYellow, ledGreen

    const onYellowLedRx = (data: string) => {
      console.log(`Yellow LED Received data: ${data.trim()}`);
    
      if(data && data[0] === "E") {
        console.log("Yellow LED flash end event received");
        // pass event to state machine as input
        trafficLight.handleInput("done");
      }
    };
    
    const ledRed = new Led(TrafficLight.redLedId, params.socket);
    const ledYellow = new Led(TrafficLight.yellowLedId, params.socket, onYellowLedRx);
    const ledGreen = new Led(TrafficLight.greenLedId, params.socket);

    const onLedReceive = (rxFrame: Frame) => {
      console.log(`Socket ${params.socket.getProtocol()} Received frame: ${rxFrame.toString().trim()}`);
      const data = rxFrame.getData();
      if(data && data.length > 0){
        const ledId = data[0];
        const ledData = data.slice(1);
        if(ledId === TrafficLight.redLedId){
          ledRed.handleRxData(ledData);
        } else if(ledId === TrafficLight.yellowLedId){
          ledYellow.handleRxData(ledData);
        } else if(ledId === TrafficLight.greenLedId){
          ledGreen.handleRxData(ledData);
        }
      }
    };

    params.socket.setOnReceive(onLedReceive);
    
    const greenState = new GreenStateFunction(trafficLight);
    greenState.onEnter = function() {
      console.log("Entering Green State");
      ledGreen.on();
      this.pressed = false;
      this.active = false;
      this.timeout = setTimeout(() => {
        if(this.pressed){
          console.log("Green state timeout - transitioning to Yellow");
          this.getStateMachine().changeState(TrafficLight.YELLOW);
        } else {
          console.log("Green state timeout - no press detected, remaining in Green");
        }
        this.active = true;
      }, params.greenDuration_mS); // 5 second timeout for green state
    };
    greenState.onExit = () => {
      console.log("Exiting Green State");
      ledGreen.off();
    };
    greenState.handleInput = function(input: string) {
      if(input === "press"){
        this.pressed = true;
        if(this.active){  
          console.log("Green state pressed again - transitioning to Yellow");
          this.getStateMachine().changeState(TrafficLight.YELLOW);
        }
      }
    };

    const yellowState = new StateFunction(trafficLight);
    yellowState.onEnter = () => {
      console.log("Entering Yellow State");
      const onPeriods = Math.floor(params.flashOn_mS / config.PERIOD_mS); // Convert ms to PERIOD_mS units
      const offPeriods = Math.floor(params.flashOff_mS / config.PERIOD_mS); // Convert ms to PERIOD_mS units

      ledYellow.flash(params.numFlashes, onPeriods, offPeriods, true); // Flash yellow LED for 3 cycles with 4 on and 4 off periods, final flash ends with LED off
    };
    yellowState.onExit = () => {
      console.log("Exiting Yellow State");
    };
    yellowState.handleInput = function(input: string) {
      if(input === "done"){
        console.log("Yellow state received flash end event - transitioning to Red");
        this.getStateMachine().changeState(TrafficLight.RED);
      }
    };

    const redState = new RedStateFunction(trafficLight);
    redState.onEnter = function() {
      console.log("Entering Red State");
      ledRed.on();
      this.timeout = setTimeout(() => {
        console.log("Red state timeout - transitioning to Green");
        this.getStateMachine().changeState(TrafficLight.GREEN);
      }, params.redDuration_mS);
    };
    redState.onExit = () => {
      console.log("Exiting Red State");
      ledRed.off();
    };
    redState.handleInput = function(input: string) {
      // ignore inputs in red state
    };

    trafficLight.addFunction(TrafficLight.GREEN, greenState);
    trafficLight.addFunction(TrafficLight.YELLOW, yellowState);
    trafficLight.addFunction(TrafficLight.RED, redState);

    trafficLight.init(ledRed, ledYellow, ledGreen);
        
    return new TrafficLightComponents(ledRed, ledYellow, ledGreen, trafficLight);
  }

}  