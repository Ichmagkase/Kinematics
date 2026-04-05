using Godot;
using System;
using System.Collections.Generic;

namespace Game.Server.Udp
{
	public partial class UdpServer : Node
	{
		private const string BIND_ADDRESS = "127.0.0.1";
		private const int PORT = 4242;

		private const string SERVICE_NAME = "UdpServer";

		private HashSet<string> actions = new HashSet<string>() {
			"move_left", "move_right", "move_jump", "move_down", "attack_1", "attack_2", "block"
		};

		private PacketPeerUdp peer = new PacketPeerUdp();

		// Called when the node enters the scene tree for the first time.
		public override void _Ready()
		{
			Error result = peer.Bind(PORT, BIND_ADDRESS);
			if (result != Error.Ok)
			{
				Game.Utils.Logger.PrintErr(
					SERVICE_NAME, $"Could not bind to IP address {BIND_ADDRESS} at port {PORT}"
				);
			}
		}

		// Called every frame. 'delta' is the elapsed time since the previous frame.
		public override void _Process(double delta)
		{
			EventBus.Instance.EmitSignal(EventBus.SignalName.PlayerTwoAction, "attack_2");

			if (peer.GetAvailablePacketCount() > 0)
			{
				byte[] packetBytes = peer.GetPacket();
				string packetMsg = System.Text.Encoding.UTF8.GetString(packetBytes).TrimEnd('\n');

				// Packet message format [player, integer 1-indexed]|[action]
				string[] playerAction = packetMsg.Split("|");
				if (playerAction.Length != 2)
				{
					Game.Utils.Logger.PrintErr(
						SERVICE_NAME, $"Message not in correct format: {packetMsg}"
					);
				}
				else if ((playerAction[0] != "1" && playerAction[0] != "2") || !actions.Contains(playerAction[1]))
				{
					Game.Utils.Logger.PrintErr(
						SERVICE_NAME,
						$"Message contains malformed input: {packetMsg}; Player: {playerAction[0]}, Action: {playerAction[1]}"
					);
				}

				if (playerAction[0] == "1")
				{
					EventBus.Instance.EmitSignal(EventBus.SignalName.PlayerOneAction, playerAction[1]);
				}
				else if (playerAction[0] == "2")
				{
					EventBus.Instance.EmitSignal(EventBus.SignalName.PlayerTwoAction, playerAction[1]);
				}
			}
		}

		public override void _ExitTree()
		{
			peer.Close();
		}
	}
}
