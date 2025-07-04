import express from "express";
import path from "path";
import { fileURLToPath } from "url";

const __filename = fileURLToPath(import.meta.url);
const __dirname = path.dirname(__filename);

const app = express();
const port = 3000;

// Serve static files from the 'www' directory
app.use(express.static(path.join(__dirname, "../../../www")));

// Route for the root path to serve index.html
app.get("/", (_, res) => {
    res.sendFile(path.join(__dirname, "index.html"));
});

// Start the server
app.listen(port, () => {
    console.log(`Server listening at http://localhost:${port}`);
});
