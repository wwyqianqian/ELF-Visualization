const path = require("path");
const fs = require("fs");

export default function handler(req, res) {
  const binariesFolder = path.resolve(process.cwd(), "binaries");
  const files = fs.readdirSync(binariesFolder);
  res.status(200).json(files);
}
