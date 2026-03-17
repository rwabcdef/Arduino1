import { SerLink, CLI, Frame, Socket, RxDataHandler, StateFunction, StateMachine, delay } from "../SerLink/SerLink";
import { Led, Motor } from "../SerLink/HardMod/Std";
import { TrafficLight, TrafficLightFactory } from "../SerLink/HardMod/StateMachines";

const serLink = new SerLink(true);

export const tlExample1 = async (portName: string, baudRate: number) => {
  console.log("main start");

  // Initialize SerLink (which includes opening the serial port and setting up reader/writer)
  await serLink.init(portName, baudRate);

  await delay(2000); // Wait a moment for the serial connection to stabilize

  const ledSocket = serLink.acquireSocket("LED01", 342);

  const tlc = TrafficLightFactory.create(ledSocket as Socket);

  //tlc.ledYellow.enableFlashEndEvent(); // Enable flash end event for yellow LED

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
  // Setup console input handler
  
  const cli = new CLI();
  
  const kbHandler = async (line: string) => {
    const trimmedLine = line.trim();
  
    if (trimmedLine === "q") {
      console.log("Exiting...");
      await tlc.ledGreen.off(); // Ensure traffic light is turned off before exiting

      cli.close();
      process.exit(0);
      return;
    }
  
    // if (trimmedLine.startsWith("l")) {
    //   sendLedCmd(trimmedLine);
    //   return;
    // } else if (trimmedLine.startsWith("m")) {
    //   sendMotorCmd(trimmedLine);
    //   return;
    // }
  
    console.log("Unknown command:", trimmedLine);
  };
  
  cli.addLineHandler(kbHandler);
  //--------------------------------

  await tlc.trafficLight.begin();
}