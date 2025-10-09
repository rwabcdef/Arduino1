/*
npm init -y
npm install serialport
npm install --save-dev typescript @types/node
npx tsc --init
npm install --save-dev ts-node

npx tsc
node dist/index.js

npm run quick-start
*/

import { SerLink } from "./SerLink/SerLink";
import * as readline from "readline";


// Adjust this to match your device
const portName = "COM3"; // or "/dev/ttyUSB0" on Linux/macOS
const baudRate = 19200;

const serLink = new SerLink(portName, baudRate, true);