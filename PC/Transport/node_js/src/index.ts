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

import { SerLink, CLI, Frame, Socket, RxDataHandler } from "./SerLink/SerLink";
import { Led, Motor } from "./SerLink/HardMod/Std";
import * as readline from "readline";

import { socketExample1 } from "./examples/led";
import { tlExample1 } from "./examples/trafficlight";

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

//socketExample1(portName, baudRate).
tlExample1(portName, baudRate).
catch(err => {
  console.error("Error in socketExample1:", err);
});
