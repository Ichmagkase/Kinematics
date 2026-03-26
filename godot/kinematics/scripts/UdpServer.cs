using Godot;
using System;

public partial class UdpServer : Node
{	
	/*
	private const string BIND_ADDRESS = "localhost";
	private const int PORT = 4242;
	
	private PacketPeerUDP peer = new PacketPeerUDP();
	
	// Called when the node enters the scene tree for the first time.
	public override void _Ready()
	{
		Error result = peer.Bind(PORT, BIND_ADDRESS);
		if (result != Error.Ok)
		{
			GD.PrintErr($"UdpServer Error: {e.Message}");
		}
	}

	// Called every frame. 'delta' is the elapsed time since the previous frame.
	public override void _Process(double delta)
	{
		if (peer.GetAvailablePacketCount() > 0)
		{
			byte[] packetBytes = peer.GetPacket();
			string event = System.Text.Encoding.UTF8.GetString(packetBytes);
			GD.Print($"Event received: {event}");
		}
	}
	*/
}
