.PHONY: all check test lint run clean

all:
	cargo build --release

check:
	cargo check

test:
	cargo test

lint:
	cargo clippy --all-targets -- -D warnings

run:
	cargo run --release -- $(ARGS)

clean:
	cargo clean
