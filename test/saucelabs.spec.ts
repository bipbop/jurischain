import humanInterval from 'human-interval'

import { createDriver, ie11, latestChrome, latestFirefox, latestSafari } from './create-driver'
import initServer from './server'

jest.setTimeout(humanInterval('30 minutes') as number)

describe('saucelabs tests', function() {
  it('should call \'jurischain\' event ', async function() {
    const baseUrl = await initServer()
    const builderConfiguration = ie11
    const driver = await createDriver(undefined, undefined, builderConfiguration)

    try {
      await driver.get(baseUrl)

      await driver.wait(async driver => {
        const element = await driver.findElement( { className: 'jurischain-captcha__text' })
        const text = await element.getText()

        if (text === 'Seu acesso foi validado com sucesso.') {
          expect(text).toBe('Seu acesso foi validado com sucesso.')

          return element
        }
      });
    } finally {
      driver.quit()
    }
  })
})
