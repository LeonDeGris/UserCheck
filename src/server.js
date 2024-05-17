const express = require('express');
const open = require('open');
const path = require('path');
const userPrivilegeAddon = require('./build/Release/user_privilege');

const app = express();
const port = 3000;

app.use(express.static('public'));
app.use(express.urlencoded({ extended: true }));

app.get('/', (req, res) => {
    res.sendFile(path.join(__dirname, 'public', 'index.html'));
});

app.post('/check', (req, res) => {
    const username = req.body.username;
    const privilege = userPrivilegeAddon.checkUserPrivilege(username);
    if (privilege === 'None') {
        res.send(`Пользователя ${username} нет`);
    } else {
        res.send(`Пользователь ${username} имеет привилегию ${privilege}`);
    }
});

app.listen(port, () => {
    console.log(`Server is running on http://localhost:${port}`);
    open(`http://localhost:${port}`);
});
