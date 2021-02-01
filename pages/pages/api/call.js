const { execFile } = require("child_process");
const path = require("path");

export default function handler(req, res) {
  let args = [];
  if (req.query && req.query.args) {
    args = JSON.parse(req.query.args);
  }

  const binary = path.resolve(process.cwd(), "elf_parser");
  execFile(binary, [...args], (error, stdout, stderr) => {
    if (error) {
      throw error;
    }
    res.status(200).send(stdout);
  });
}
