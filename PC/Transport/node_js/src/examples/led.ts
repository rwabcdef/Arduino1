import { SerLink, App, Frame, Socket, RxDataHandler, Led, Motor,
  StateFunction, StateMachine
 } from "../SerLink/SerLink";
import * as readline from "readline";

const greenLedId = 'G';
const redLedId = 'R';
const yellowLedId = 'Y';

const WAIT = "WAIT";
const ACTIVE = "ACTIVE";
const ledSM = new StateMachine();

const serLink = new SerLink(true);

//--------------------------------
// Leds

const onLedReceive = (rxFrame: Frame) => {
  console.log(`Socket LED01 Received frame: ${rxFrame.toString().trim()}`);
  const data = rxFrame.getData();
  if (data) {
    if (data.length > 0) {
      // Resolve LED instance based on first character of data (e.g. G for green, R for red, Y for yellow)
      const ledId = data.charAt(0).toUpperCase();
      const led = getLed(ledId);
      if (led) {
        led.handleRxData(data);
      } else {
        console.warn(`Received LED frame for unknown LED ID: ${ledId}`);
      }
    }
  }
};

const ledSocket = serLink.acquireSocket("LED01", 342, onLedReceive);

const onYellowLedRx = (data: string) => {
  console.log(`Yellow LED Received data: ${data.trim()}`);

  if(data && data[1] === "E") {
    console.log("Yellow LED flash end event received");
    // pass event to state machine as input
    ledSM.handleInput("done");
  }
};

const greenLed = new Led(greenLedId, ledSocket);
const yellowLed = new Led(yellowLedId, ledSocket, onYellowLedRx);
const redLed = new Led(redLedId, ledSocket);

//--------------------------------
// Button socket and handler
const onButtonReceive = (rxFrame: Frame) => {
  console.log(`Socket BUT01 Received frame: ${rxFrame.toString().trim()}`);

  const data = rxFrame.getData();
  if (data && data[1] === "P") {
    console.log("Button pressed");
    
    // pass event to state machine as input
    ledSM.handleInput("go");
  }
}

const buttonSocket = serLink.acquireSocket("BUT01", 123, onButtonReceive);
//--------------------------------
// Motor socket and instance

const motorSocket = serLink.acquireSocket("MOTOR", 456);
const motorA = new Motor("A", motorSocket);

//--------------------------------
// Setup console input handler

const cli = new App.Console.CLI();

const kbHandler = (line: string) => {
  const trimmedLine = line.trim();

  if (trimmedLine === "q") {
    console.log("Exiting...");
    cli.close();
    process.exit(0);
    return;
  }

  if (trimmedLine.startsWith("l")) {
    sendLedCmd(trimmedLine);
    return;
  } else if (trimmedLine.startsWith("m")) {
    sendMotorCmd(trimmedLine);
    return;
  }

  console.log("Unknown command:", trimmedLine);
};

cli.addLineHandler(kbHandler);

//--------------------------------
// state machine example implementation (demonstrates how to create a state machine and add states)

const waitState = new StateFunction(ledSM);
waitState.onEnter = () => {
  console.log("Entered WAIT state");
};
waitState.onExit = () => {
  console.log("Exiting WAIT state");
  yellowLed.flash(3, 5, 5).catch(err => console.error("Error flashing yellow LED:", err));
};
waitState.handleInput = function (input: string): void {
  if (input === "go") {
    this.getStateMachine().changeState(ACTIVE); 
  }
};

const activeState = new StateFunction(ledSM);
activeState.onEnter = () => {
  console.log("Entered ACTIVE state");
};
activeState.onExit = () => {
  console.log("Exiting ACTIVE state");
};
activeState.handleInput = function (input: string): void {
  if (input === "done") {
    this.getStateMachine().changeState(WAIT); 
  }
};

ledSM.addFunction(WAIT, waitState);
ledSM.addFunction(ACTIVE, activeState);
ledSM.start(WAIT); // start in WAIT state

//--------------------------------

export const socketExample1 = async (portName: string, baudRate: number) => {
  console.log("main start");

  // Initialize SerLink (which includes opening the serial port and setting up reader/writer)
  await serLink.init(portName, baudRate);

  setTimeout(() => {

    // Enable yellow LED flash end event by default
    yellowLed.enableFlashEndEvent();
  }, 2000);
};

// Start main and catch any errors
// main().catch(err => {
//   console.error("Error in main:", err);
// });

//-----------------------------------------------------------------------------------------
const getLed = (ledId: string): Led | null => {
  switch (ledId.toUpperCase()) {
    case greenLedId:
      return greenLed;
    case redLedId:
      return redLed;
    case yellowLedId:
      return yellowLed;
    default:
      return null;
  }
};

// LED command examples:
// lg1         - turn green LED on
// lg0         - turn green LED off
// lr1         - turn red LED on
// lr0         - turn red LED off
// ly1         - turn yellow LED on
// ly0         - turn yellow LED off
// lgf,5,4,8   - flash green LED 5 times, 4 on periods, 8 off periods
// lrf,3,7,12  - flash red LED 3 times, 7 on periods, 12 off periods
// lyf,2,5,5   - flash yellow LED 2 times, 5 on periods, 5 off periods
// lys         - enable yellow LED flash end event
// lyc         - cancel yellow LED flash end event
//
async function sendLedCmd(input: string): Promise<void> {
  const trimmed = input.trim();

  // lg0, lg1, lr0, lr1 ...
  let match = trimmed.match(/^l([a-zA-Z])([01SsCc])$/);
  if (match) {
    const ledId = match[1].toUpperCase();
    const state = match[2].toUpperCase();

    const led = getLed(ledId);
    if (!led) {
      console.error("Invalid LED ID:", ledId);
      return;
    }

    if (state === "0") {
      led.off()
        .then(() => console.log(`Turned off LED ${ledId}`))
        .catch(err => console.error(`Error turning off LED ${ledId}:`, err));
    } else if (state === "1") {
      led.on()
        .then(() => console.log(`Turned on LED ${ledId}`))
        .catch(err => console.error(`Error turning on LED ${ledId}:`, err));
    } else if (state === "S") {
       led.enableFlashEndEvent().then(() =>
         console.log(`Enabled flash end event for LED ${ledId}`))
         .catch(err => console.error(`Error enabling flash end event for LED ${ledId}:`, err));
    } else if (state === "C") {
       led.cancelFlashEndEvent().then(() =>
         console.log(`Disabled flash end event for LED ${ledId}`))
         .catch(err => console.error(`Error disabling flash end event for LED ${ledId}:`, err));
    }


    // const payload = `${led}${state}`;

    // ledSocket?.sendData(payload, true)
    //   .then(() => console.log(`Sent ${payload}`))
    //   .catch(err => console.error("LED send error:", err));


    return;
  }

  // lgf,5,4,8  |  lrf,3,7,12 ...
  match = trimmed.match(/^l([a-zA-Z])f,(\d+),(\d+),(\d+)$/);
  if (match) {
    const ledId = match[1].toUpperCase();
    const numFlashes = Number(match[2]);
    const onPeriods = Number(match[3]);
    const offPeriods = Number(match[4]);

    const pad2 = (n: number) => n.toString().padStart(2, "0");

    const led = getLed(ledId);
    if (!led) {
      console.error("Invalid LED ID:", ledId);
      return;
    }

    led.flash(numFlashes, onPeriods, offPeriods)
      .then(() => console.log(`Flashing LED ${ledId}: ${numFlashes} times, ${onPeriods} on, ${offPeriods} off`))
      .catch(err => console.error(`Error flashing LED ${ledId}:`, err));

    return;
  }

  console.error("Invalid LED command:", input);
}

const getMotor = (motorId: string): Motor | null => {
  switch (motorId.toUpperCase()) {
    case "A":
      return motorA;
    default:
      return null;
  }
}

// Motor command examples:
// map35    - set motor A percent to 35%
// map100   - set motor A percent to 100%
// madf     - set motor A direction to forward
// madr     - set motor A direction to reverse
// madd     - set motor A direction to disabled
// maf0     - set motor A frequency to 500 Hz
// maf1     - set motor A frequency to 1 kHz
// maf2     - set motor A frequency to 2 kHz
// magp     - get motor A percent
// magd     - get motor A direction
// magf     - get motor A frequency
async function sendMotorCmd(input: string): Promise<void> {
  const trimmed = input.trim();

  //
  // map35, map100 ...
  //
  let match = trimmed.match(/^m([a-zA-Z])p(\d{1,3})$/);
  if (match) {
    const motorId = match[1].toUpperCase();
    const percent = Number(match[2]);

    const motor = getMotor(motorId);
    if (!motor) {
      console.error("Invalid motor ID:", motorId);
      return;
    }

    const status = await motor.setPercent(percent);
    if (status === 0) {
      console.log(`Motor ${motorId} set to ${percent}%`);
    } else {
      console.error(`Motor ${motorId} percent error:`, status);
    }
    return;
  }

  //
  // madf, madr, madd
  //
  match = trimmed.match(/^m([a-zA-Z])d([frd])$/i);
  if (match) {
    const motorId = match[1].toUpperCase();
    const dirCode = match[2].toUpperCase();

    const motor = getMotor(motorId);
    if (!motor) {
      console.error("Invalid motor ID:", motorId);
      return;
    }

    let direction: string;
    switch (dirCode) {
      case "F":
        direction = Motor.MOTOREVENT__DIRECTION_FORWARD;
        break;
      case "R":
        direction = Motor.MOTOREVENT__DIRECTION_REVERSE;
        break;
      case "D":
        direction = Motor.MOTOREVENT__DIRECTION_DISABLED;
        break;
      default:
        console.error("Invalid direction code:", dirCode);
        return;
    }

    const status = await motor.setDirection(direction);
    if (status === 0) {
      console.log(`Motor ${motorId} direction = ${direction}`);
    } else {
      console.error(`Motor ${motorId} direction error:`, status);
    }
    return;
  }

  //
  // maf0, maf1, maf2, maf3, maf4, maf5
  //
  match = trimmed.match(/^m([a-zA-Z])f([0-5])$/);
  if (match) {
    const motorId = match[1].toUpperCase();
    const freqCode = match[2];

    const motor = getMotor(motorId);
    if (!motor) {
      console.error("Invalid motor ID:", motorId);
      return;
    }

    const status = await motor.setFrequency(freqCode);

    if (status === 0) {
      console.log(`Motor ${motorId} frequency set to code ${freqCode}`);
    } else {
      console.error(`Motor ${motorId} frequency error:`, status);
    }
    return;
  }

  // get motor A percent/direction/frequency examples:
  // magp     - get motor A percent
  // magd     - get motor A direction
  // magf     - get motor A frequency
  // magp, magd, magf
  match = trimmed.match(/^m([a-zA-Z])g([pdf])$/i);
  if (match) {
    console.log("match:", match);
    const motorId = match[1].toUpperCase();
    const param = match[2].toUpperCase();

    const motor = getMotor(motorId);
    if (!motor) {
      console.error("Invalid motor ID:", motorId);
      return;
    }

    switch (param) {
      case "P":
        const percent = await motor.getPercent();
        if (percent !== null) {
          console.log(`Motor ${motorId} percent = ${percent}%`);
        } else {
          console.error(`Motor ${motorId} get percent error`);
        }
        break;
      case "D":
        const direction = await motor.getDirection();
        if (direction !== null) {
          console.log(`Motor ${motorId} direction = ${direction}`);
        } else {
          console.error(`Motor ${motorId} get direction error`);
        }
        break;
      case "F":
        const frequency = await motor.getFrequency();
        if (frequency !== null) {
          console.log(`Motor ${motorId} frequency code = ${frequency}`);
        } else {
          console.error(`Motor ${motorId} get frequency error`);
        }
        break;
      default:
        console.error("Invalid parameter code:", param);
    }

    return;
  }

  console.error("Invalid motor command:", input);
}

//-----------------------------------------------------------------------------------------