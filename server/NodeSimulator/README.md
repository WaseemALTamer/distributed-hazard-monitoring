THIS IS WHERE THE VIRTUAL NODE SIMULATOR IS LOCATED THIS IS RESPONSIBLE 
FOR SIMULATING WHAT A REAL  ESP32 NODE  WOULD DO FOR REPORTING MESSAGES


NOTE THAT FOR THE REAL NODE YOU SHOULD BE ABLE TO COMMUNICATE TO IT AND
TELL IT WHICH STATE IT SHOULD BE  IN BUT FOR  VIRTUAL NODE YOU WILL NOT
BE ABLE TO COMMNUICATE DATA WITH IT  JUST A  NODE THAT WILL REPORT DATA


I LEFT THE IDEA OF IT BEING RESPONDING TO  THE STATE  BECAUSE YOU WOULD
THEN HAVE TO LISTEN ON TWO PORTS OTHER WISE YOU WOULD HAVE TO ESTABLISH
THAT BEFORE HAND AND THIS  VIRTAUL NODE  SIMULATOR IS  NOT SUPPORT THAT
BECAUSE ITIS NOT THAT SOPHISTICATED


IF YOU ARE INTRESTED IN MAKING IT RESPONDE  TO YOUR MESSAGES YOU CAN DO
IT THOUGH PUSH IT TO A DIFFERENT GIT BRANCH UNRIL I REVIEW IT 

NOTE THAT THE UNITS AND THE VALUES OF THE SIMULATED  VALUES WILL CHANGE
LATER ON DOWN THE LINE


HOW TO RUN:

RUN THIS FOR SIMPLISITY:
    "python main.py --normal 10 --dangerous 10" 


CHECK THE PRAMATERS OUT FOR MORE CONTROL
```
    python main.py -h
    usage: main.py [-h] [--normal NORMAL] [--dangerous DANGEROUS] [--interval INTERVAL] [--port PORT]

    Broadcast virtual sensor nodes over UDP.

    options:
    -h, --help            show this help message and exit
    --normal NORMAL       number of normal nodes (default: 3)
    --dangerous DANGEROUS
                            number of dangerous nodes (default: 1)
    --interval INTERVAL   seconds between broadcasts (default: 10)
    --port PORT           UDP destination port (default: 39394)
```

INTERACTIVE DEMO COMMANDS (WHILE RUNNING):
Type any of the following in the terminal and press Enter:
- `danger <node_number>` : switch node `<node_number>` (1-based) to dangerous readings and broadcast immediately (e.g. `danger 2`)
- `normal <node_number>` : switch node `<node_number>` (1-based) back to normal readings and broadcast immediately (e.g. `normal 2`)
- `status` (or `list`)   : list all simulated nodes, their MAC addresses, and their current mode