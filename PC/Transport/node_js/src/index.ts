/*
npm init -y
npm install serialport
npm install --save-dev typescript @types/node
npx tsc --init
npm install --save-dev ts-node

cd PC/Transport/node_js/

npx tsc
node dist/index.js

npm run quick-start

----------------------
# generate node_modules dir:
npm install 
*/

import { SerLink, App, Frame, Socket, RxDataHandler, Led } from "./SerLink/SerLink";
import * as readline from "readline";

console.log('index start');


// type callback = (str: string) => number;

// const cb = (str: string): number => {
//   console.log(`Callback called with string: ${str}`);
//   return str.length;
// };

// const f1 = (cb: callback, input: string): number => {
//   return cb(input);
// };

// const result = f1(cb, "Hello, World!");
// console.log(`Result from callback: ${result}`);

// // Exit after testing callback

// process.exit(0);

//-----------------------------------------------------------------------------------------


// Adjust this to match your device
const portName = "/dev/ttyACM0"; // linux arduino uno r3
//const portName = "COM3"; // Windows arduino uno r3
//const portName = "/dev/ttyUSB0"; // linux usb-serial adapter (STM32 uart etc.)
const baudRate = 19200;

const greenLedId = 'G';
const redLedId = 'R';
const yellowLedId = 'Y';

const serLink = new SerLink(true);
serLink.init(portName, baudRate);

const onLedReceive = (rxFrame: Frame) => {
  console.log(`Socket LED01 Received frame: ${rxFrame.toString().trim()}`);
  const data = rxFrame.getData();
  if (data) {
    if (data.length > 0) {
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
};

const greenLed = new Led(greenLedId, ledSocket);
const yellowLed = new Led(yellowLedId, ledSocket, onYellowLedRx);
const redLed = new Led(redLedId, ledSocket);

const onButtonReceive = (rxFrame: Frame) => {
  console.log(`Socket BUT01 Received frame: ${rxFrame.toString().trim()}`);
}

const buttonSocket = serLink.acquireSocket("BUT01", 123, onButtonReceive);

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
  }

  console.log("Unknown command:", trimmedLine);
};

cli.addLineHandler(kbHandler);

//--------------------------------

// delay main to allow SerLink to initialize
setTimeout(() => {
  main().catch(err => {
    console.error("Error in main:", err);
  });
}, 2000);

//--------------------------------

const main = async () => {
  console.log("main start");
  // Enable yellow LED flash end event by default
  yellowLed.enableFlashEndEvent();
}


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

    // const payload =
    //   `${led}F` +
    //   pad2(numFlashes) +
    //   pad2(onPeriods) +
    //   pad2(offPeriods);

    // ledSocket?.sendData(payload, true)
    //   .then(() => console.log(`Sent ${payload}`))
    //   .catch(err => console.error("LED send error:", err));

    return;
  }

  console.error("Invalid LED command:", input);
}


//-----------------------------------------------------------------------------------------