-- todo: skip when table already exists
CREATE TABLE if not exists task (
    taskId      SERIAL PRIMARY KEY,
    name        VARCHAR(100) NOT NULL,
    createdOn   TIMESTAMP NOT NULL,
    completedOn TIMESTAMP
);

