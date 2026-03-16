import { SerLink, CLI, Frame, Socket, RxDataHandler, StateFunction, StateMachine } from "../SerLink/SerLink";
import { Led, Motor } from "../SerLink/HardMod/Std";
import { TrafficLight, TrafficLightFactory } from "../SerLink/HardMod/StateMachines";

const serLink = new SerLink(true);

export const tlExample1 = async (portName: string, baudRate: number) => {
  console.log("main start");

  // Initialize SerLink (which includes opening the serial port and setting up reader/writer)
  await serLink.init(portName, baudRate);

  const ledSocket = serLink.acquireSocket("LED01", 342);

  const tlc = TrafficLightFactory.create(ledSocket as Socket);

  //tlc.trafficLight.init(tlc.ledRed, tlc.ledYellow, tlc.ledGreen);
  
  //--------------------------------
  // Button socket and handler
  const onButtonReceive = (rxFrame: Frame) => {
    console.log(`Socket BUT01 Received frame: ${rxFrame.toString().trim()}`);
  
    const data = rxFrame.getData();
    if (data && data[1] === "P") {
      console.log("Button pressed");
      
      // pass event to state machine as input
      tlc.trafficLight.handleInput("press");
    }
  }
  
  const buttonSocket = serLink.acquireSocket("BUT01", 123, onButtonReceive);
  //--------------------------------

  tlc.trafficLight.begin();
}