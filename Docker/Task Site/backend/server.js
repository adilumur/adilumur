const { Pool } = require('pg');

const env = process.env;
const config = {
    host: env.DB_HOST,
    port: env.DB_PORT || '5432',
    user: env.DB_USER,
    password: env.DB_PASSWORD,
    database: env.DB_NAME,
}; // odkud se asi tak berou tyto údaje? ;)

const pool = new Pool(config);

async function query(query, params) {
    const {rows, fields} = await pool.query(query, params);
    return rows;
}

const express = require('express');
var app = express();

app.use(express.json());
app.use(express.urlencoded({ extended: false }));

const cors = require('cors');
app.use(cors({
    origin: 'http://localhost:3000' // todo: napsat sem stránku, ze které k nám smí chodit requesty (cross origin resource...)
}));

app.get('/tasks', async function(req, res, next) {
    const tasks = await query(
        'select taskId, name, createdOn, completedOn from task order by taskId asc;', // todo: vyber všechny úkoly seřazené od nejnovějšího
        []
    );
    res.json({"tasks": tasks ? tasks : []});
});

app.put('/tasks', async function(req, res, next) {
    const tasks = await query(
        'INSERT INTO task (name, createdOn) VALUES ($1, NOW()) RETURNING taskId, name, createdOn, completedOn;',
        [ req.body.name ]
    );
    res.json(tasks ? tasks[0] : {});
});

app.patch('/task/:taskId', async function(req, res) {
    await query(
        'update task set completedOn = now() WHERE taskId = $1;', // todo: nastavit completedOn na aktuální čas pro daný úkol
        [ req.params.taskId ]
    );
    res.json({});
});

app.delete('/task/:taskId', async function(req, res) {
    await query(
        'delete from task WHERE taskId = $1;', // todo: smazat daný úkol
        [ req.params.taskId ]
    );
    res.json({});
});

app.listen(80);


