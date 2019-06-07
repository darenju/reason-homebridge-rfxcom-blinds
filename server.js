const express = require('express');
const app = express();
const port = 4000;

app.use(express.static("build"));

app.listen(port, () => {
  console.log(`Now ready on port ${port}…`);
});
