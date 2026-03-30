using Godot;
using System.Linq;
using System.Collections.Generic;

namespace Game.Player
{
	public partial class Player : CharacterBody2D
	{
		public int PlayerId;
		public PlayerSprite _playerSprite;
		private PlayerConfig _playerConfig;
		private bool _hasDoubleJumpped = false;
		private readonly string[] attackActions = 
		{
			PlayerInputActions.Attack1,
			PlayerInputActions.Attack2,
		};
		private readonly HashSet<Player> _overlappingPlayers = new();
		private readonly HashSet<Player> _playersHitThisAttack = new();

		// Since we can receive input from signals and Godot Inputs, we need some protection
		private readonly object _inputLock = new();
		private struct PendingInput
		{
			public bool Jump;
			public bool Attack1;
			public bool Attack2;
			public bool Block;
			public Vector2 Movement;
		}
		private PendingInput _pendingInput;

		private float _health;
		public float Health
		{
			get => _health;
			set
			{
				_health = value;
				if (_health <= 0)
					OnDeath();
			}
		}
		public bool OfficallyDied = false;		
		public float BlockHealth;
		public float BlockCooldown;
		public bool OnBlockCooldown;
		public float Damage;

		public void HandleActionSignal(string e)
		{
			// The logic will be handled in _PhysicsProcess()
			{
				switch (e)
				{
					case PlayerInputActions.Jump:      _pendingInput.Jump = true;                    break;
					case PlayerInputActions.MoveLeft:  _pendingInput.Movement += new Vector2(-1, 0); break;
					case PlayerInputActions.MoveRight: _pendingInput.Movement += new Vector2(1, 0);  break;
					case PlayerInputActions.MoveDown:  _pendingInput.Movement += new Vector2(0, 1);  break;
					case PlayerInputActions.Attack1:   _pendingInput.Attack1 = true;                 break;
					case PlayerInputActions.Attack2:   _pendingInput.Attack2 = true;                 break;
					case PlayerInputActions.Block:     _pendingInput.Block = true;                   break;
				}
			}
		}

		public override void _Ready()
		{
			if (PlayerId <= 0)
			{
				GD.PrintErr($"PlayerId not set (Id {PlayerId}, expected > 0). This should be done by the GameStart.cs script");
			}
			_playerConfig = GetNode<PlayerConfig>("PlayerConfig");
			_playerSprite = GetNode<PlayerSprite>("AnimatedSprite2D");

			Health = _playerConfig.Health;
			BlockHealth = _playerConfig.BlockHealth;
			BlockCooldown = _playerConfig.BlockCooldown;
			OnBlockCooldown = false;
			Damage = _playerConfig.Damage;

			var area = GetNode<Area2D>("Area2D");
			area.BodyEntered += OnBodyEntered;
			area.BodyExited += OnBodyExited;

			// Copy to avoid shared shapes across players
			var areaShape = area.GetNode<CollisionShape2D>("CollisionShape2D");
			var originalRect = (RectangleShape2D)areaShape.Shape;
			var newRect = new RectangleShape2D();
			newRect.Size = originalRect.Size;
			areaShape.Shape = newRect;
		}

		public override void _PhysicsProcess(double delta)
		{
			if (Health < 0) return;

			HandleGravity(delta);
			lock (_inputLock);

			// Merge Godot inputs and signals
			bool jump = Input.IsActionJustPressed(PlayerInputActions.Jump) || _pendingInput.Jump;

			Vector2 keyboardDir = Input.GetVector(
				PlayerInputActions.MoveLeft, PlayerInputActions.MoveRight,
				PlayerInputActions.MoveUp,   PlayerInputActions.MoveDown);
			Vector2 movement = keyboardDir + _pendingInput.Movement;

			string? triggeredAction =
				attackActions.FirstOrDefault(a => Input.IsActionPressed(a))
				?? (_pendingInput.Attack1 ? PlayerInputActions.Attack1 : null)
				?? (_pendingInput.Attack2 ? PlayerInputActions.Attack2 : null);

			bool blockPressed = Input.IsActionJustPressed(PlayerInputActions.Block) || _pendingInput.Block;
			bool isBlocking = _playerSprite.GetCurrentAnimationName() == _playerSprite.BlockAnimationName;

			// clear signal state for next frame
			_pendingInput = default;

			if (_playerSprite.PlayingAttackingAnimation)
			{
				foreach (var p in _overlappingPlayers)
				{
					if (!_playersHitThisAttack.Contains(p))
					{
						HandlePlayerAttackOnPlayer(p);
						_playersHitThisAttack.Add(p);
					}
				}
			}
			else
			{
				_playersHitThisAttack.Clear();
			}


			if (OnBlockCooldown)
			{
				HandleBlockCooldown(delta);
			}

			// End block if blocking and another action is given
			if (isBlocking && (jump || triggeredAction != null || movement != Vector2.Zero))
				HandleEndBlock();

			if (jump) HandleJump();
			HandleMovement(movement);
			if (triggeredAction != null) HandleAttack(triggeredAction);
			if (blockPressed && !isBlocking && !OnBlockCooldown) HandleBlock();

			UpdateAnimation();
			MoveAndSlide();
		}

		private void HandleGravity(double delta)
		{
			if (!IsOnFloor())
				Velocity += GetGravity() * (float)delta;
				if (Velocity.Y > 0)
				{
					Velocity += GetGravity() * (float)delta;
				}
		}

		private void HandleJump()
		{
			bool canJump = IsOnFloor();
			bool canDoubleJump = !IsOnFloor()
				&& !_hasDoubleJumpped
				&& _playerConfig.CanDoubleJump;

			if (!canJump && !canDoubleJump) return;

			_hasDoubleJumpped = !canJump; // true if double jumping, false if normal jump

			Velocity = new Vector2(Velocity.X, _playerConfig.JumpVelocity);
		}

		private void HandleAttack(string action)
		{
			if (!IsOnFloor()) return;
			if (action == PlayerInputActions.Attack1)
			{
				_playerSprite.Attack1();
				return;
			}
			if  (action == PlayerInputActions.Attack2)
			{
				_playerSprite.Attack2();
				return;
			}
		}

		private void HandleBlock()
		{
			if (!IsOnFloor() || Velocity != Vector2.Zero) return;
			_playerSprite.Block();
		}

		private void HandleEndBlock()
		{
			_playerSprite.EndBlock();
		}

		private void HandleBlockCooldown(double delta)
		{
			BlockCooldown -= (float)delta;
			if (BlockCooldown <= 0)
			{
				BlockCooldown = _playerConfig.BlockCooldown;
				BlockHealth = _playerConfig.BlockHealth;
				OnBlockCooldown = false;
			}
		}
		private void HandleMovement(Vector2 direction)
		{
			if (direction != Vector2.Zero)
				Velocity = new Vector2(direction.X * _playerConfig.Speed, Velocity.Y);
			else
				Velocity = new Vector2(Mathf.MoveToward(Velocity.X, 0, _playerConfig.Speed), Velocity.Y);
		}

		private void UpdateAnimation()
		{
			bool onFloor = IsOnFloor();
			bool isFalling = !onFloor && Velocity.Y >= 0;
			bool isRising = !onFloor && Velocity.Y < 0;
			bool isMoving = Velocity.X != 0;

			if (isFalling)  { _playerSprite.JumpDown(); return; }
			if (isRising)   { _playerSprite.JumpUp();   return; }
			if (isMoving)
			{
				if (Velocity.X < 0) _playerSprite.FaceLeft();
				else if (Velocity.X > 0) _playerSprite.FaceRight();
				_playerSprite.Run();
				return;
			}

			_playerSprite.Idle();
		}

		private void HandlePlayerAttackOnPlayer(Player otherPlayer)
		{
			var isFacingTowardMe = otherPlayer._playerSprite.FlipH != _playerSprite.FlipH;
			var theyAreBlocking = otherPlayer._playerSprite.PlayingBlockingAnimation;

			if (!theyAreBlocking /* || theyAreBlocking && !isFacingTowardMe*/)
			{
				otherPlayer.Health -= _playerConfig.Damage;
			}
			else if (theyAreBlocking)
			{
				otherPlayer.BlockHealth -= 1;
				if (otherPlayer.BlockHealth <= 0)
				{
					GD.Print($"PlayerId[{PlayerId}]: Handling Block Cooldown!");
					otherPlayer.Health -=  _playerConfig.Damage * 1.2f;
					otherPlayer.OnBlockCooldown = true;
					otherPlayer._playerSprite.EndBlock();
				}
			}

			GD.Print($"Player[{PlayerId}]: Health: {Health}");
			GD.Print($"Player[{otherPlayer.PlayerId}]: Health: {otherPlayer.Health}");

		}

		private void OnBodyEntered(Node2D body)
		{
			if (body != this && body is Player otherPlayer)
				_overlappingPlayers.Add(otherPlayer);
		}

		private void OnBodyExited(Node2D body)
		{
			if (body is Player otherPlayer)
				if (_overlappingPlayers.Contains(otherPlayer))
				{
					_overlappingPlayers.Remove(otherPlayer);
				}
		}

		private void OnDeath()
		{
			_playerSprite.Death();
		}

	}
}
