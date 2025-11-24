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
const portName = "/dev/ttyACM0"; // or "/dev/ttyUSB0" on Linux/macOS , COM3
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
  }
  else if (trimmedLine === "lg1") {
    const frame = new Frame("LED01", Frame.TYPE_TRANSMISSION, 67, 2, "G1");
    //serLink.sendFrame(frame).then(() => {
    ledSocket?.sendData("G1", true).then(() => {
      console.log("Sent LG1 command");
    }).catch((err) => {
      console.error("Error sending LG1 command:", err);
    });
  }
  else if (trimmedLine === "lg0") {
    const frame = new Frame("LED01", Frame.TYPE_TRANSMISSION, 67, 2, "G0");
    //serLink.sendFrame(frame).then(() => {
    ledSocket?.sendData("G0", true).then(() => {
      console.log("Sent LG0 command");
    }).catch((err) => {
      console.error("Error sending LG0 command:", err);
    });
  }
}

cli.addLineHandler(kbHandler);