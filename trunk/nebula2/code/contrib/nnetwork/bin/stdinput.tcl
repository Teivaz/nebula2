#-------------------------------------------------------------------------------
#   Defines the standard input mappings for NetworkDemo
#-------------------------------------------------------------------------------

set oldCwd [psel]

sel /sys/servers/input
    .beginmap
    .map "keyb0:q.up"          "exit"
    .map "keyb0:s.down"        "init_server"
    .map "keyb0:c.down"        "init_client"
    .map "keyb0:j.down"        "join"
    .map "keyb0:x.down"        "start"
    .map "keyb0:m.down"        "message"
    .map "keyb0:up.down"       "index_up"
    .map "keyb0:down.down"     "index_down"
    .map "keyb0:f11.down"      "console"
    .endmap
sel ..

sel $oldCwd
