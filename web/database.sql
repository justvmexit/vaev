-- Just the users yk
CREATE TABLE users(
    id VARCHAR(36) NOT NULL UNIQUE,
    username VARCHAR(32) NOT NULL UNIQUE,
    password VARCHAR(255) NOT NULL,
    role INT NOT NULL DEFAULT 0,
    created_at DATETIME DEFAULT CURRENT_TIMESTAMP
);

-- Pretty obvious but just all bots
CREATE TABLE zombies(
    id VARCHAR(36) NOT NULL UNIQUE,
    ip VARCHAR(15) NOT NULL, -- Note: This only supports IPv4
    owner VARCHAR(36) NOT NULL,
    architecture VARCHAR(32) NOT NULL, -- Note: Zombie info, fx. Windows 10 x64
    last_ping DATETIME DEFAULT CURRENT_TIMESTAMP,
    infected_on DATETIME DEFAULT CURRENT_TIMESTAMP
);

-- This is domains that point towards API.
CREATE TABLE domains(
    id VARCHAR(36) NOT NULL UNIQUE,
    domain VARCHAR(255) NOT NULL,
    added_on DATETIME DEFAULT CURRENT_TIMESTAMP
);


-- Reversal attempts / Suspicious activity
CREATE TABLE suspicious_activity(
    id VARCHAR(36) NOT NULL UNIQUE,
    info VARCHAR(64) NOT NULL,
    ip VARCHAR(15) NOT NULL,
    attempted_on DATETIME DEFAULT CURRENT_TIMESTAMP
)

-- Logs of zombie activity
CREATE TABLE zombie_activity(
    id VARCHAR(36) NOT NULL UNIQUE,
    info VARCHAR(64) NOT NULL,
    added_on DATETIME DEFAULT CURRENT_TIMESTAMP
)

-- Due actions for zombies
CREATE TABLE actions(
    id VARCHAR(36) NOT NULL UNIQUE,
    action INT NOT NULL,
    data VARCHAR(64),
    zombie VARCHAR(36) NOT NULL,
    added_on DATETIME DEFAULT CURRENT_TIMESTAMP
);
