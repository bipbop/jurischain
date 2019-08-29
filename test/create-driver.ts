import { Builder, Capabilities } from 'selenium-webdriver'

const tags = ['jurischain']

export type BuilderConfiguration = {
  browserName: string
  browserVersion: string
  platformName: string
  screenWidth: number
  screenHeight: number
  screenshotHeight: number
  screenshotWidth: number
}

export const ie11: BuilderConfiguration = {
  browserName: 'internet explorer',
  browserVersion: '11',
  platformName: 'Windows 10',
  screenWidth: 1280,
  screenHeight: 960,
  screenshotWidth: 1264,
  screenshotHeight: 874,
}

export const latestChrome: BuilderConfiguration = {
  browserName: 'chrome',
  browserVersion: 'latest',
  platformName: 'Windows 10',
  screenWidth: 1280,
  screenHeight: 960,
  screenshotWidth: 1264,
  screenshotHeight: 828,
}

export const latestFirefox: BuilderConfiguration = {
  browserName: 'firefox',
  browserVersion: 'latest',
  platformName: 'Windows 10',
  screenWidth: 1280,
  screenHeight: 960,
  screenshotWidth: 1268,
  screenshotHeight: 879,
}

export const latestSafari: BuilderConfiguration = {
  browserName: 'safari',
  browserVersion: '12',
  platformName: 'macOS 10.13',
  screenWidth: 1280,
  screenHeight: 960,
  screenshotWidth: 1280,
  screenshotHeight: 922,
}

export async function createDriver(
  withCapabilities: {} | Capabilities = {},
  envName?: string,
  { browserName, browserVersion, platformName, screenWidth, screenHeight }: BuilderConfiguration = latestChrome,
) {
  return new Builder()
    .withCapabilities({
      browserName,
      browserVersion,
      platformName,
      ...withCapabilities,
      'sauce:options': {
        username: process.env.SAUCELABS_USERNAME,
        accessKey: process.env.SAUCELABS_ACCESSKEY,
        build: 'Jurischain',
        name: envName,
        maxDuration: 3600,
        idleTimeout: 1000,
        tags: tags,
        screenResolution: `${screenWidth}x${screenHeight}`,
      },
    })
    .usingServer('https://ondemand.saucelabs.com/wd/hub')
    .build()
}
