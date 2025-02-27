FROM node:14

WORKDIR /usr/src/app

# install deps
COPY package*.json ./

RUN npm install

# bundle app source
COPY . .

EXPOSE 8080
CMD ["node", "server.js"]