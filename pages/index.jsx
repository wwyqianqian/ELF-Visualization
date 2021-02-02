import {
  AppBar,
  Container,
  Grid,
  List,
  ListItem,
  ListItemText,
  ListSubheader,
  ListItemIcon,
  Toolbar,
  Typography,
} from "@material-ui/core";
import PlayArrowIcon from "@material-ui/icons/PlayArrow";
import { useEffect, useState } from "react";

const operations = [
  {
    label: "ELF header",
    op: "-h",
  },
  {
    label: "Section headers",
    op: "-S",
  },
  {
    label: "Program headers",
    op: "-l",
  },
  {
    label: "Symbol table",
    op: "-s",
  },
  {
    label: "Relocations",
    op: "-r",
  },
];

function HomePage() {
  const [files, setFiles] = useState([]);
  const [usingOperation, setUsingOperation] = useState();
  const [usingFile, setUsingFile] = useState();
  const [waiting, setWaiting] = useState(false);
  const [stdout, setStdout] = useState("");

  const exec = async () => {
    if (waiting || !usingOperation || !usingFile) {
      return;
    }
    setWaiting(true);
    setStdout("Waiting…");
    const args = JSON.stringify([usingOperation, `binaries/${usingFile}`]);
    const response = await (
      await fetch(`/api/call?args=${escape(args)}`)
    ).text();

    setStdout(response);
    setWaiting(false);
  };

  useEffect(() => {
    (async () => {
      const files = JSON.parse(await (await fetch("/api/list")).text());
      console.log(files);
      setFiles(files);
    })();
  }, []);

  return (
    <div
      style={{
        display: "flex",
        flexDirection: "column",
        alignContent: "space-between",
        height: "100vh",
      }}
    >
      <AppBar position="relative">
        <Toolbar>
          <Typography variant="h6">ELF Parser</Typography>
        </Toolbar>
      </AppBar>

      <Container maxWidth="lg" style={{ padding: 0, flexGrow: 1 }}>
        <div style={{ paddingTop: 20, height: "100%", maxHeight: "100%" }}>
          <Grid
            container
            spacing={3}
            style={{ height: "100%", maxHeight: "100%" }}
          >
            <Grid item xs={3}>
              <List component="nav">
                <ListItem button onClick={() => exec()}>
                  <ListItemIcon>
                    <PlayArrowIcon />
                  </ListItemIcon>
                  <ListItemText primary="Execute" />
                </ListItem>
              </List>

              <List
                subheader={
                  <ListSubheader component="div" id="nested-list-subheader">
                    Operations
                  </ListSubheader>
                }
                component="nav"
              >
                {operations.map((operation) => (
                  <ListItem
                    button
                    selected={usingOperation === operation.op}
                    onClick={() => setUsingOperation(operation.op)}
                  >
                    <ListItemText primary={operation.label} />
                  </ListItem>
                ))}
              </List>
              <List
                subheader={
                  <ListSubheader component="div" id="nested-list-subheader">
                    Binaries
                  </ListSubheader>
                }
                component="nav"
              >
                {files.map((file) => (
                  <ListItem
                    button
                    selected={usingFile === file}
                    onClick={() => setUsingFile(file)}
                  >
                    <ListItemText primary={file} />
                  </ListItem>
                ))}
              </List>
            </Grid>
            <Grid
              item
              xs={9}
              style={{
                background: "#272C35",
                color: "white",
                fontFamily: "monospace",
                overflowY: "scroll",
                height: "100%",
                maxHeight: 800,
              }}
            >
              <pre>{stdout}</pre>
            </Grid>
          </Grid>
        </div>
      </Container>
    </div>
  );
}

export default HomePage;
