npm init -y
npm install serialport
npm install --save-dev typescript @types/node
npx tsc --init
npm install --save-dev ts-node

npx tsc
node dist/index.js

----------------------------------------
# add to scripts section of: package.json

"scripts": {
    "test": "echo \"Error: no test specified\" && exit 1",
    "quick-start": "ts-node src/index.ts",
    "build": "tsc",
    "start": "npm run build && node dist/index.js"
  },

----------------------------------------
modify file: tsconfig.json  

{
  "compilerOptions": {
    "target": "ES2020",
    "module": "commonjs",
    "strict": true,
    "esModuleInterop": true,
    "outDir": "./dist"
  },
  "include": ["src"]
}

----------------------------------------
# create file src/index.ts

npm run quick-start