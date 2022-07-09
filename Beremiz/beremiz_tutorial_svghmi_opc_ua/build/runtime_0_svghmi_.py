
# TODO : multiple watchdog (one for each svghmi instance)
def svghmi_0_watchdog_trigger():
    Popen(['echo', 'Watchdog', 'for', 'svghmi_0', '!'])

max_svghmi_sessions = 16

def _runtime_0_svghmi_start():
    global svghmi_watchdog, svghmi_servers

    srv = svghmi_servers.get("localhost:8008", None)
    if srv is not None:
        svghmi_root, svghmi_listener, path_list = srv
        if 'svghmi_0' in path_list:
            raise Exception("SVGHMI svghmi_0: path svghmi_0 already used on localhost:8008")
    else:
        svghmi_root = Resource()
        factory = HMIWebSocketServerFactory()
        factory.setProtocolOptions(maxConnections=16)

        svghmi_root.putChild("ws", WebSocketResource(factory))

        svghmi_listener = reactor.listenTCP(8008, Site(svghmi_root), interface='localhost')
        path_list = []
        svghmi_servers["localhost:8008"] = (svghmi_root, svghmi_listener, path_list)

    svghmi_root.putChild(
        'svghmi_0',
        NoCacheFile('svghmi_0.xhtml',
            defaultType='application/xhtml+xml'))

    path_list.append("svghmi_0")

    Popen(['chromium', 'http://localhost:8008/svghmi_0#watchdog'])

    if True:
        if svghmi_watchdog is None:
            svghmi_watchdog = Watchdog(
                30,
                5,
                svghmi_0_watchdog_trigger)
        else:
            raise Exception("SVGHMI svghmi_0: only one watchdog allowed")


def _runtime_0_svghmi_stop():
    global svghmi_watchdog, svghmi_servers

    if svghmi_watchdog is not None:
        svghmi_watchdog.cancel()
        svghmi_watchdog = None

    svghmi_root, svghmi_listener, path_list = svghmi_servers["localhost:8008"]
    svghmi_root.delEntity('svghmi_0')

    path_list.remove('svghmi_0')

    if len(path_list)==0:
        svghmi_root.delEntity("ws")
        svghmi_listener.stopListening()
        svghmi_servers.pop("localhost:8008")

    pass # no command given

        