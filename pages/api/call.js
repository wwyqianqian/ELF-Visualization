const { spawn } = require("child_process");
const path = require("path");

export default function handler(req, res) {
  let args = [];
  if (req.query && req.query.args) {
    args = JSON.parse(req.query.args);
  }

  const binary = path.resolve(process.cwd(), "elf_parser");
  const program = spawn(binary, [...args]);

  program.stdout.on("data", (data) => {
    res.status(200).write(data);
  });

  program.stderr.on("data", (data) => {
    res.status(200).write(data);
  });

  program.on("exit", () => {
    res.status(200).end();
  });
}