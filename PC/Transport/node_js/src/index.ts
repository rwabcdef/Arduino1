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

import { SerLink } from "./SerLink/SerLink";
import * as readline from "readline";


// Adjust this to match your device
const portName = "/dev/ttyACM0"; // or "/dev/ttyUSB0" on Linux/macOS , COM3
const baudRate = 19200;

const serLink = new SerLink(true);
serLink.init(portName, baudRate);