extern crate mio;
extern crate rustls;

use mio::{Events, Poll, Interest, Token};
use mio::net::TcpStream;

use std::net::{self, SocketAddr};

// Token for our listening socket.
const LISTENER: mio::Token = mio::Token(0);

fn main() {
    let version = env!("CARGO_PKG_NAME").to_string() + ", version: " + env!("CARGO_PKG_VERSION");

    let mut addr: std::net::SocketAddr = "0.0.0.0:8123".parse().unwrap();

    let mut listener = mio::net::TcpListener::bind(addr).expect("cannot listen on port");
    let mut poll = match Poll::new() {
        Ok(poll) => poll,
        Err(e) => panic!("failed to create Poll instance; err={:?}", e),
    };

    poll.registry().register(&mut listener, LISTENER, mio::Interest::READABLE).unwrap();

    let mut tlsserv = TlsServer::new(listener);

    let mut events = mio::Events::with_capacity(256);
    loop {
        poll.poll(&mut events, None)
            .unwrap();

        for event in events.iter() {
            match event.token() {
                LISTENER => {
                    tlsserv.accept(poll.registry())
                        .expect("error accepting socket");
                }
                _ => tlsserv.conn_event(poll.registry(), &event)
            }
        }
    }
}