import asyncio
import websockets
import struct

async def receive_data(uri):
    async with websockets.connect(uri) as websocket:
        while True:
            data = await websocket.recv()
            process_received_data(data)

def process_received_data(data):
    # Przetwarzanie odebranych danych
    # W tym przypadku przyjmujemy, że dane to struktura LiDARFrameTypeDef
    lidar_frame = struct.unpack('!BBHH' + 'HBB'*12 + 'HHB', data)
    print("Odebrano dane:", lidar_frame)

if __name__ == "__main__":
    server_uri = "ws://adres_ip_esp8266:80/ws"  # Zastąp "adres_ip_esp8266" rzeczywistym adresem IP ESP8266

    asyncio.get_event_loop().run_until_complete(receive_data(server_uri))