using Godot;
using System;

namespace Game.Client.UDP
{
	public partial class UdpServer : Node
	{
		private const string BIND_ADDRESS = "localhost";
		private const int PORT = 4242;
		private PacketPeerUdp peer = new PacketPeerUdp();

		public override void _Ready()
		{
			Error result = peer.Bind(PORT, BIND_ADDRESS);
			if (result != Error.Ok)
			{
				GD.PrintErr($"UdpServer Error: Failed to bind, code: {result}");
			}
		}

		public override void _Process(double delta)
		{
			if (peer.GetAvailablePacketCount() > 0)
			{
				byte[] packetBytes = peer.GetPacket();
				string receivedEvent = System.Text.Encoding.UTF8.GetString(packetBytes);
				GD.Print($"Event received: {receivedEvent}");
			}
		}
	}
}
