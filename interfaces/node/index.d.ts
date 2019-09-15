export default class Jurischain {
  constructor(difficult: number, seed: string);
  challengeResponse(response: string): boolean;
  readChallenge(): string;
  solveStep(): boolean;
  verify(): boolean;
}