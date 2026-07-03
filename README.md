# Order Book Simulator

![C++](https://img.shields.io/badge/C%2B%2B-20-00599C?style=for-the-badge&logo=cplusplus&logoColor=white)
![Asio](https://img.shields.io/badge/Network-Asio%20Standalone-red?style=for-the-badge&logo=cplusplus&logoColor=white)
![Platform](https://img.shields.io/badge/Platform-Windows%20%7C%20Linux%20%7C%20macOS-lightgrey?style=for-the-badge)
![License](https://img.shields.io/badge/License-MIT-green?style=for-the-badge)

A C++20 limit order book simulator with:
- A matching engine (`OrderBook`)
- Pre-trade risk checks (`RiskManager`)
- Protocol abstraction via gateways (`IGateway`)
- FIX parsing/writing (`FIXParser`, `FIXWriter`)
- Async TCP server built on standalone Asio
- Unit tests with GoogleTest

This repository builds three binaries:
- `OrderBookConsole`: interactive local console flow
- `OrderBookServer`: TCP server for FIX messages
- `OrderBookTests`: unit tests

## Project Layout

```
include/
	gateways/        # Gateway interfaces + FIX gateway definitions
	network/         # TCP server/session interfaces
	orchestrator/    # Exchange orchestration layer
	orderbook/       # Core order and matching engine interfaces
	parsers/         # Parser interfaces + FIX/JSON parser headers
	risk/            # Risk manager interface
	writers/         # Writer interfaces + FIX writer header
src/
	gateways/
	network/
	orchestrator/
	orderbook/
	parsers/
	risk/
	writers/
	main_console.cpp
	main_server.cpp
tests/
	orderbook_tests.cpp
	fix_protocol_tests.cpp
	risk_check_tests.cpp
tcp_test.py        # Simple Python TCP client for end-to-end FIX testing
```

## Architecture Overview

1. `TCPSession` receives raw bytes from a client.
2. `ExchangeOrchestrator::on_data_received` forwards payload to the configured gateway.
3. `FIXGateway` parses FIX into an `Order`.
4. `RiskManager` validates the order.
5. `OrderBook` assigns an ID, places/matches, and triggers execution callbacks.
6. `FIXWriter` emits execution reports back to the active session.

### Architecture Diagram
![Project Architecture](/diagrams/architecture.svg)

In console mode, the same orchestration path is exercised without a network session.

## Prerequisites

- CMake >= 3.20
- A C++20-capable compiler
	- Windows: MSVC (Visual Studio Build Tools or Visual Studio)
	- Linux/macOS: Clang or GCC
- Python 3.x (optional, only for `tcp_test.py`)

Dependencies are fetched automatically by CMake:
- Asio (`ASIO_STANDALONE`)
- GoogleTest

## Build

From the repository root:

### Windows (PowerShell)

```powershell
cmake -S . -B build
cmake --build build --config Debug
```

### Linux/macOS

```bash
cmake -S . -B build
cmake --build build
```

## Run

### 1) Interactive Console

Windows (Debug config):

```powershell
.\build\Debug\OrderBookConsole.exe
```

Example prompt input:

```text
LIMIT BUY 100 10 42
MARKET SELL 100 5 13
```

Input format:

```text
(LIMIT|MARKET) (BUY|SELL) Price Qty ClientID
```

### 2) TCP Server (FIX over TCP)

Start server (port `8080`):

```powershell
.\build\Debug\OrderBookServer.exe
```

Then run the Python test client from repo root:

```powershell
python .\tcp_test.py
```

The script sends a resting SELL order then an aggressive BUY order and prints returned execution reports.

## Test

### Run the unit test binary directly

Windows (Debug config):

```powershell
.\build\Debug\OrderBookTests.exe
```

### Or run through CTest

```powershell
ctest --test-dir build -C Debug --output-on-failure
```

Current tests cover:
- Core order-book flows (`LIMIT`, IOC path)
- FIX parser/writer behavior
- Risk validation rules

## FIX Notes

The FIX implementation uses SOH (`\x01`) as field delimiter.

Common tags used by this codebase include:
- `8` BeginString
- `9` BodyLength
- `10` Checksum
- `11` ClOrdID
- `35` MsgType (`D` NewOrderSingle, `8` ExecutionReport)
- `37` OrderID
- `38` Quantity
- `39` OrdStatus
- `40` OrdType (`1` Market, `2` Limit)
- `44` Price
- `54` Side (`1` Buy, `2` Sell)
- `60` Timestamp
- `150` ExecType
- `151` LeavesQty

When printing FIX for readability, this project often replaces SOH with `|` in logs.

## Known Gaps / Next Improvements

- `JSONParser` is currently a placeholder.
- Order cancellation API exists in interfaces but is not fully wired through the TCP flow.
- Risk throttling (`orders/sec`) is declared but not yet enforced.
- More tests can be added for market data output and network edge cases.

## Development Tips

- Keep parsing/writing logic protocol-specific inside gateway/parser/writer classes.
- Keep matching/risk logic protocol-agnostic inside `OrderBook` and `RiskManager`.
- For repeatable local validation, use this loop:
	1. Build
	2. Run `OrderBookTests`
	3. Run `OrderBookServer`
	4. Run `tcp_test.py`
