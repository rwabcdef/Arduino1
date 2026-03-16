import { SerLink, CLI, Frame, Socket, RxDataHandler, StateFunction, StateMachine } from "../../SerLink/SerLink";
import { Led } from "./Std";

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

  public begin(){
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

export class TrafficLightFactory {

  public static create(socket: Socket): TrafficLightComponents {

    const trafficLight = new TrafficLight();  // ledRed, ledYellow, ledGreen

    const onYellowLedRx = (data: string) => {
      console.log(`Yellow LED Received data: ${data.trim()}`);
    
      if(data && data[1] === "E") {
        console.log("Yellow LED flash end event received");
        // pass event to state machine as input
        trafficLight.handleInput("done");
      }
    };
    
    const ledRed = new Led(TrafficLight.redLedId, socket);
    const ledYellow = new Led(TrafficLight.yellowLedId, socket, onYellowLedRx);
    const ledGreen = new Led(TrafficLight.greenLedId, socket);

    const onLedReceive = (rxFrame: Frame) => {
      console.log(`Socket ${socket.getProtocol()} Received frame: ${rxFrame.toString().trim()}`);
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

    socket.setOnReceive(onLedReceive);
    trafficLight.init(ledRed, ledYellow, ledGreen);

    const greenState = new StateFunction(trafficLight);
    greenState.onEnter = () => {
      console.log("Entering Green State");
      ledGreen.on();
    };
    greenState.onExit = () => {
      console.log("Exiting Green State");
      ledGreen.off();
    };
    greenState.handleInput = function(input: string) {
      if(input === "press"){
        this.getStateMachine().changeState(TrafficLight.RED);
      }
    };

    const redState = new StateFunction(trafficLight);
    redState.onEnter = () => {
      console.log("Entering Red State");
      ledRed.on();
    };
    redState.onExit = () => {
      console.log("Exiting Red State");
      ledRed.off();
    };
    redState.handleInput = function(input: string) {
      if(input === "press"){
        this.getStateMachine().changeState(TrafficLight.GREEN);
      }
    };

    trafficLight.addFunction(TrafficLight.GREEN, greenState);
    trafficLight.addFunction(TrafficLight.RED, redState);

    trafficLight.init(ledRed, ledYellow, ledGreen);
        
    return new TrafficLightComponents(ledRed, ledYellow, ledGreen, trafficLight);
  }

}  