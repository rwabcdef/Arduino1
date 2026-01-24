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

import { SerLink, App, Frame, Socket } from "./SerLink/SerLink";
import * as readline from "readline";


// Adjust this to match your device
const portName = "/dev/ttyACM0"; // linux arduino uno r3
//const portName = "COM3"; // Windows arduino uno r3
//const portName = "/dev/ttyUSB0"; // linux usb-serial adapter (STM32 uart etc.)
const baudRate = 19200;

const serLink = new SerLink(true);
serLink.init(portName, baudRate);

const ledSocket = serLink.acquireSocket("LED01", 342);

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

//-----------------------------------------------------------------------------------------
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
function sendLedCmd(input: string) {
  const trimmed = input.trim();

  // lg0, lg1, lr0, lr1 ...
  let match = trimmed.match(/^l([a-zA-Z])([01SsCc])$/);
  if (match) {
    const led = match[1].toUpperCase();
    const state = match[2].toUpperCase();

    const payload = `${led}${state}`;

    ledSocket?.sendData(payload, true)
      .then(() => console.log(`Sent ${payload}`))
      .catch(err => console.error("LED send error:", err));

    return;
  }

  // lgf,5,4,8  |  lrf,3,7,12 ...
  match = trimmed.match(/^l([a-zA-Z])f,(\d+),(\d+),(\d+)$/);
  if (match) {
    const led = match[1].toUpperCase();
    const numFlashes = Number(match[2]);
    const onPeriods = Number(match[3]);
    const offPeriods = Number(match[4]);

    const pad2 = (n: number) => n.toString().padStart(2, "0");

    const payload =
      `${led}F` +
      pad2(numFlashes) +
      pad2(onPeriods) +
      pad2(offPeriods);

    ledSocket?.sendData(payload, true)
      .then(() => console.log(`Sent ${payload}`))
      .catch(err => console.error("LED send error:", err));

    return;
  }

  console.error("Invalid LED command:", input);
}
