struct TlsServer {
    server: TcpListener,
    connections: HashMap<mio::Token, Connection>,
    next_id: usize,
    tls_config: Arc<rustls::ServerConfig>,
}

impl TlsServer {
    fn new(server: TcpListener) -> TlsServer {
        TlsServer {
            server,
            connections: HashMap::new(),
            next_id: 2,
            tls_config: cfg,
        }
    }

    fn accept(&mut self, registry: &mio::Registry) -> Result<(), io::Error> {
        loop {
            match self.server.accept() {
                Ok((socket, add)) => {
                    debug!("Accepting new connection from {:?}", addr);

                    let tls_session = rustls::ServerSession::new(&self.tls_config);
                    let mode = self.mode.clone();

                    let token = mio::Token(self.next_id);
                    self.next_id += 1;

                    let mut connection = Connection::new(socket, token, mode, tls_session);
                    connection.register(registry);
                    self.connections.insert(token, connection);
                }
            }
            Err(ref err) if err.kind() == io::ErrorKind::WouldBlock => return Ok(()),
            Err(err) => {
                println!("encountered error while accepting connection; err={:?}", err);
                return Err(err);
            }
        }
    }

    fn conn_event(&mut self, registry: &mio::Registry, event: &mio::event::Event) {
        let token = event.token();

        if self.connections.contains_key(&token) {
            self.connections
                .get_mut(&token)
                .unwrap()
                .ready(registry, event);

            if self.connections[&token].is_closed() {
                self.connections.remove(&token);
            }
        }
    }
}