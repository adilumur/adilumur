### Start work
```
docker-compose up -d --build
docker exec -it mongo bash
mongosh
use admin
db.auth("root","heslo")
use cases
db.createUser( { user: "student",
                 pwd: "student",
                 roles: [ { role: "clusterAdmin", db: "admin" },
                          { role: "readAnyDatabase", db: "admin" },
                          "readWrite"] }
)
db.auth("student","student")
show dbs 
show collections
```



### Import collections example
```
mongoimport -u student -p student -d cases -c cases /home/mongodb_cases.json
```

### Queries example

> select * from cases
``` 
db.cases.find( {} ) 
```

> show third element with RequestorFirstName "Bond"
``` 
db.cases.find( { "RequestorFirstName" : "Bond" } ).limit(1).skip(2) 
```

> How many Bonds are in DB
``` 
db.cases.count( { "RequestorFirstName" : "Bond"} ) 
```

> How many Bonds with age 36 or 47 
```
db.cases.count( { "RequestorFirstName" : "Bond", "gender" : "male", $or: [{"age" : 47}, {"age" : 36}] } )
```

> If we want to have result formatted
```
db.cases.find( {"RequestorFirstName" : "Bond"} ).pretty()
```

> Min and Max age in whole collection
```
db.cases.aggregate([
    { "$group": {
        "_id": null,
        "max": { "$max": "$age" },
        "min": { "$min": "$age" }
    }}
])
```

> What case statuses are available in DB 
```
db.cases.aggregate([
  { "$group": {
        "_id": "$CaseCurrentStatus"
  }}
])
```

> How many those statuses
```
db.cases.aggregate([
  { "$group": {
        "_id": "$CaseCurrentStatus",
        count: { $sum: 1 }
  }}
])
```

> Combination of search conditions with the number in individual groups
```
db.cases.aggregate([
    { $match: { "CaseCurrentStatus": "pending"}  },
    { "$group": {
        "_id": "$gender",
        "max": { "$max": "$age" },
        "min": { "$min": "$age" }
    }}
])
```

> Sorting ( 1=asc, -1=desc )
```
db.cases.find( {"RequestorFirstName" : "Bond"} ).sort({"age":-1}).pretty()
```

> Execution plan ( .explain() )
```
db.cases.find( {"RequestorFirstName" : "Bond"} ).sort({"age":-1}).explain()
```

> Validation scheme
```
db.runCommand({
    collMod: "studenti",
    validator: {
        $jsonSchema: {
            bsonType: "object",
            required: [ "jmeno", "prijmeni", "vek", "rocnik", "predmety", "pohlavi", "staleStuduje" ],
            properties: {
                jmeno: {
                    bsonType: "string",
                    description: "Jméno musí být string!"
                },
                prijmeni: {
                    bsonType: "string",
                    description: "Příjmení musí být string!"
                },
                pohlavi: {
                    bsonType: "string",
                    "enum": [ "Muž", "Žena" ],
                    description: "Pohlavi musí být buď Muž, nebo Žena"
                },
                rocnik: {
                    bsonType: "int",
                    minimum: 1,
                    maximum: 5,
                    description: "Ročník musí být v intervalu od 1 do 5"
                },
                vek: {
                    bsonType: "int",
                    minimum: 6
                },
                staleStuduje: {
                    bsonType: "bool",
                },
                predmety: {
                    bsonType: "array",
                    uniqueItems: true,
                    items: { bsonType: "string" }
                }
            }
        }
    },
    validationAction: "error"
});
```

> Insert
```
db.studenti.insertMany([
    {
        jmeno: "Jan",
        prijmeni: "Novák",
        rocnik: 1,
        vek: 18,
        staleStuduje: true,
        pohlavi: "Muž",
        predmety: [ "BI-CAO", "BI-ZMA" ]
    }
]);
```

> Update
```
db.studenti.updateMany( { vek: { $gt: 26 }}, {
    $set : { staleStuduje: false },
    $unset : { predmety: "" },
});
```

> Inner Join with table "predmety"
```
db.studenti.aggregate([
    { $lookup: {
        "from": "predmety",
        "localField": "predmety",
        "foreignField": "zkratka",
        "as": "predmety_detail"
    }}
])
```

> Delete
```
db.studenti.deleteMany( { staleStuduje: false } );
```

> Drop collection
```
db.studenti.drop()
```
