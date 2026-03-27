using Godot;
using System;

public partial class UdpServer : Node
{	
	private const string _BIND_ADDRESS = "localhost";
	private const int _PORT = 4242;
	
	private PacketPeerUdp peer = new PacketPeerUdp();

	private static void PrintErr(string info)
	{
		GD.PrintErr($"UdpServer Error: {info}");
	}
	
	// Called when the node enters the scene tree for the first time.
	public override void _Ready()
	{
		Error result = peer.Bind(_PORT, _BIND_ADDRESS);
		if (result != Error.Ok)
		{
			PrintErr($"Could not bind to IP address {_BIND_ADDRESS} at port {_PORT}");
		}
	}

	// Called every frame. 'delta' is the elapsed time since the previous frame.
	public override void _Process(double delta)
	{
		if (peer.GetAvailablePacketCount() > 0)
		{
			byte[] packetBytes = peer.GetPacket();
			string packetMsg = System.Text.Encoding.UTF8.GetString(packetBytes);

			// Packet message format [player, integer 1-indexed]|[action]
			string[] playerAction = packetMsg.Split("|");
			if (playerAction[0] == "1")
			{
				EventBus.Instance.EmitSignal(EventBus.SignalName.PlayerOneGesture, playerAction[1]);
			}
			else if (playerAction[1] == "2")
			{
				EventBus.Instance.EmitSignal(EventBus.SignalName.PlayerTwoGesture, playerAction[1]);
			}
		}
	}
}
