import ngrok from 'ngrok'
import express from 'express'
import { join } from 'path'

const defaultPort = process.env.SERVER_PORT ?
  parseInt(process.env.SERVER_PORT, 10) :
  8080

let server: Promise<string> | undefined

export default async function initServer(port?: number) {
  if (server) return server

  const app = express()
  const usePort = port || defaultPort
  app.use(express.static(join(__dirname, '..')));
  app.listen(usePort)

  server = ngrok.connect({
    proto: 'http',
    addr: usePort
  })

  return server
}

export { initServer }
