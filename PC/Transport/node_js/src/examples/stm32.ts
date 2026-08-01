import { SerLink, CLI, Frame, Socket, RxDataHandler, StateFunction, StateMachine, delay } from "../SerLink/SerLink";

const button0Protocol = "BUT00";

const serLink = new SerLink(true);

// Basic comms with STM32 device using SerLink
export const stm32SerLink = async (portName: string, baudRate: number) => {
  console.log("main start");

  await serLink.init(portName, baudRate);
  
  await delay(2000); // Wait a moment for the serial connection to stabilize

  //--------------------------------
  // Button socket and handler
  const onButtonReceive = (rxFrame: Frame) => {
    console.log(`Socket ${button0Protocol} Received frame: ${rxFrame.toString().trim()}`);
  
    
  }
  
  const buttonSocket = serLink.acquireSocket(button0Protocol, 123, onButtonReceive);

  const debugSocket = serLink.acquireSocket("DBG00", 342);

  //--------------------------------
  // Setup console input handler
  
  const cli = new CLI();
  
  const kbHandler = async (line: string) => {
    const trimmedLine = line.trim();
  
    if (trimmedLine === "q") {
      console.log("Exiting...");

      cli.close();
      process.exit(0);
      return;
    }
    else if (trimmedLine === "d") {
      console.log("Sending debugSocket data...");
      debugSocket?.sendData("zxc", true);
      return;
    }
    else if (trimmedLine === "r") {
      console.log("Sending debugSocket data...");
      debugSocket?.sendData("R1", true);
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

};