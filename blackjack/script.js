class BlackjackGame {
    constructor() {
        this.deck = [];
        this.playerHand = [];
        this.dealerHand = [];
        this.gameOver = false;
        this.dealerHidden = true;
        
        this.initializeElements();
        this.setupEventListeners();
        this.newGame();
    }
    
    initializeElements() {
        this.dealerCardsEl = document.getElementById('dealer-cards');
        this.playerCardsEl = document.getElementById('player-cards');
        this.dealerScoreEl = document.getElementById('dealer-score');
        this.playerScoreEl = document.getElementById('player-score');
        this.messageEl = document.getElementById('message');
        this.hitBtn = document.getElementById('hit-btn');
        this.standBtn = document.getElementById('stand-btn');
        this.newGameBtn = document.getElementById('new-game-btn');
    }
    
    setupEventListeners() {
        this.hitBtn.addEventListener('click', () => this.hit());
        this.standBtn.addEventListener('click', () => this.stand());
        this.newGameBtn.addEventListener('click', () => this.newGame());
    }
    
    createDeck() {
        const suits = ['♠', '♥', '♦', '♣'];
        const ranks = ['A', '2', '3', '4', '5', '6', '7', '8', '9', '10', 'J', 'Q', 'K'];
        this.deck = [];
        
        for (let suit of suits) {
            for (let rank of ranks) {
                this.deck.push({ suit, rank });
            }
        }
        
        this.shuffleDeck();
    }
    
    shuffleDeck() {
        for (let i = this.deck.length - 1; i > 0; i--) {
            const j = Math.floor(Math.random() * (i + 1));
            [this.deck[i], this.deck[j]] = [this.deck[j], this.deck[i]];
        }
    }
    
    dealCard() {
        return this.deck.pop();
    }
    
    getCardValue(card) {
        if (card.rank === 'A') return 11;
        if (['J', 'Q', 'K'].includes(card.rank)) return 10;
        return parseInt(card.rank);
    }
    
    calculateHandValue(hand) {
        let value = 0;
        let aces = 0;
        
        for (let card of hand) {
            let cardValue = this.getCardValue(card);
            value += cardValue;
            if (card.rank === 'A') aces++;
        }
        
        while (value > 21 && aces > 0) {
            value -= 10;
            aces--;
        }
        
        return value;
    }
    
    displayCard(card, hidden = false) {
        const cardEl = document.createElement('div');
        cardEl.className = 'card';
        
        if (hidden) {
            cardEl.className += ' hidden';
            cardEl.textContent = '🂠';
        } else {
            cardEl.textContent = `${card.rank}${card.suit}`;
        }
        
        return cardEl;
    }
    
    updateDisplay() {
        this.dealerCardsEl.innerHTML = '';
        this.playerCardsEl.innerHTML = '';
        
        this.dealerHand.forEach((card, index) => {
            const hidden = this.dealerHidden && index === 0;
            this.dealerCardsEl.appendChild(this.displayCard(card, hidden));
        });
        
        this.playerHand.forEach(card => {
            this.playerCardsEl.appendChild(this.displayCard(card));
        });
        
        const playerScore = this.calculateHandValue(this.playerHand);
        this.playerScoreEl.textContent = `Score: ${playerScore}`;
        
        if (this.dealerHidden) {
            this.dealerScoreEl.textContent = 'Score: ?';
        } else {
            const dealerScore = this.calculateHandValue(this.dealerHand);
            this.dealerScoreEl.textContent = `Score: ${dealerScore}`;
        }
    }
    
    newGame() {
        this.createDeck();
        this.playerHand = [];
        this.dealerHand = [];
        this.gameOver = false;
        this.dealerHidden = true;
        this.messageEl.textContent = '';
        
        this.playerHand.push(this.dealCard());
        this.dealerHand.push(this.dealCard());
        this.playerHand.push(this.dealCard());
        this.dealerHand.push(this.dealCard());
        
        this.updateDisplay();
        this.checkBlackjack();
        
        this.hitBtn.disabled = false;
        this.standBtn.disabled = false;
    }
    
    hit() {
        if (this.gameOver) return;
        
        this.playerHand.push(this.dealCard());
        this.updateDisplay();
        
        const playerScore = this.calculateHandValue(this.playerHand);
        if (playerScore > 21) {
            this.endGame('Player busts! Dealer wins!');
        }
    }
    
    stand() {
        if (this.gameOver) return;
        
        this.dealerHidden = false;
        this.updateDisplay();
        
        this.dealerPlay();
    }
    
    dealerPlay() {
        const dealerScore = this.calculateHandValue(this.dealerHand);
        
        if (dealerScore < 17) {
            setTimeout(() => {
                this.dealerHand.push(this.dealCard());
                this.updateDisplay();
                this.dealerPlay();
            }, 1000);
        } else {
            this.determineWinner();
        }
    }
    
    checkBlackjack() {
        const playerScore = this.calculateHandValue(this.playerHand);
        const dealerScore = this.calculateHandValue(this.dealerHand);
        
        if (playerScore === 21 && dealerScore === 21) {
            this.dealerHidden = false;
            this.updateDisplay();
            this.endGame('Both have blackjack! It\'s a tie!');
        } else if (playerScore === 21) {
            this.dealerHidden = false;
            this.updateDisplay();
            this.endGame('Blackjack! Player wins!');
        } else if (dealerScore === 21) {
            this.dealerHidden = false;
            this.updateDisplay();
            this.endGame('Dealer has blackjack! Dealer wins!');
        }
    }
    
    determineWinner() {
        const playerScore = this.calculateHandValue(this.playerHand);
        const dealerScore = this.calculateHandValue(this.dealerHand);
        
        if (dealerScore > 21) {
            this.endGame('Dealer busts! Player wins!');
        } else if (playerScore > dealerScore) {
            this.endGame('Player wins!');
        } else if (dealerScore > playerScore) {
            this.endGame('Dealer wins!');
        } else {
            this.endGame('It\'s a tie!');
        }
    }
    
    endGame(message) {
        this.gameOver = true;
        this.messageEl.textContent = message;
        this.hitBtn.disabled = true;
        this.standBtn.disabled = true;
    }
}

const game = new BlackjackGame();