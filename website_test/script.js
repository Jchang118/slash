class WebsiteApp {
    constructor() {
        this.users = this.loadUsers();
        this.currentUser = null;
        this.messages = this.loadMessages();
        this.initializeDefaultAdmin();
        this.bindEvents();
        this.updateUI();
    }

    initializeDefaultAdmin() {
        const adminExists = this.users.some(user => user.username === 'admin');
        if (!adminExists) {
            this.users.push({
                username: 'admin',
                password: 'Eypass338',
                isAdmin: true
            });
            this.saveUsers();
        }
    }

    loadUsers() {
        const users = localStorage.getItem('jc_adventure_users');
        return users ? JSON.parse(users) : [];
    }

    saveUsers() {
        localStorage.setItem('jc_adventure_users', JSON.stringify(this.users));
    }

    loadMessages() {
        const messages = localStorage.getItem('jc_adventure_messages');
        return messages ? JSON.parse(messages) : [];
    }

    saveMessages() {
        localStorage.setItem('jc_adventure_messages', JSON.stringify(this.messages));
    }

    bindEvents() {
        // Auth form toggles
        document.getElementById('show-register').addEventListener('click', (e) => {
            e.preventDefault();
            this.showRegisterForm();
        });

        document.getElementById('show-login').addEventListener('click', (e) => {
            e.preventDefault();
            this.showLoginForm();
        });

        // Form submissions
        document.querySelector('#login-form form').addEventListener('submit', (e) => {
            e.preventDefault();
            this.handleLogin();
        });

        document.querySelector('#register-form form').addEventListener('submit', (e) => {
            e.preventDefault();
            this.handleRegister();
        });

        // Logout
        document.getElementById('logout-btn').addEventListener('click', () => {
            this.logout();
        });

        // Chat
        document.getElementById('send-btn').addEventListener('click', () => {
            this.sendMessage();
        });

        document.getElementById('message-input').addEventListener('keypress', (e) => {
            if (e.key === 'Enter') {
                this.sendMessage();
            }
        });
    }

    showRegisterForm() {
        document.getElementById('login-form').classList.remove('active');
        document.getElementById('register-form').classList.add('active');
    }

    showLoginForm() {
        document.getElementById('register-form').classList.remove('active');
        document.getElementById('login-form').classList.add('active');
    }

    handleLogin() {
        const username = document.getElementById('login-username').value.trim();
        const password = document.getElementById('login-password').value;

        if (!username || !password) {
            alert('Please fill in all fields');
            return;
        }

        const user = this.users.find(u => u.username === username && u.password === password);
        if (user) {
            this.currentUser = user;
            this.updateUI();
            this.displayMessages();
        } else {
            alert('Invalid username or password');
        }
    }

    handleRegister() {
        const username = document.getElementById('register-username').value.trim();
        const password = document.getElementById('register-password').value;
        const confirmPassword = document.getElementById('confirm-password').value;

        if (!username || !password || !confirmPassword) {
            alert('Please fill in all fields');
            return;
        }

        if (password !== confirmPassword) {
            alert('Passwords do not match');
            return;
        }

        if (username.length < 3) {
            alert('Username must be at least 3 characters long');
            return;
        }

        if (password.length < 6) {
            alert('Password must be at least 6 characters long');
            return;
        }

        if (this.users.some(u => u.username === username)) {
            alert('Username already exists');
            return;
        }

        const newUser = {
            username: username,
            password: password,
            isAdmin: false
        };

        this.users.push(newUser);
        this.saveUsers();

        alert('Account created successfully! Please login.');
        this.showLoginForm();
        
        // Clear form
        document.getElementById('register-username').value = '';
        document.getElementById('register-password').value = '';
        document.getElementById('confirm-password').value = '';
    }

    logout() {
        this.currentUser = null;
        this.updateUI();
        
        // Clear login form
        document.getElementById('login-username').value = '';
        document.getElementById('login-password').value = '';
    }

    updateUI() {
        const authSection = document.getElementById('auth-section');
        const mainSection = document.getElementById('main-section');
        const currentUserSpan = document.getElementById('current-user');

        if (this.currentUser) {
            authSection.classList.add('hidden');
            mainSection.classList.remove('hidden');
            currentUserSpan.textContent = `Welcome, ${this.currentUser.username}${this.currentUser.isAdmin ? ' (Admin)' : ''}`;
        } else {
            authSection.classList.remove('hidden');
            mainSection.classList.add('hidden');
            this.showLoginForm();
        }
    }

    sendMessage() {
        const messageInput = document.getElementById('message-input');
        const messageText = messageInput.value.trim();

        if (!messageText) return;

        const message = {
            id: Date.now(),
            username: this.currentUser.username,
            text: messageText,
            timestamp: new Date().toLocaleTimeString(),
            isAdmin: this.currentUser.isAdmin
        };

        this.messages.push(message);
        this.saveMessages();
        this.displayMessages();

        messageInput.value = '';
    }

    displayMessages() {
        const chatMessages = document.getElementById('chat-messages');
        chatMessages.innerHTML = '';

        this.messages.forEach(message => {
            const messageDiv = document.createElement('div');
            messageDiv.className = `message ${message.isAdmin ? 'admin' : ''}`;
            
            messageDiv.innerHTML = `
                <div class="username">${message.username}</div>
                <div class="timestamp">${message.timestamp}</div>
                <div class="text">${this.escapeHtml(message.text)}</div>
            `;

            chatMessages.appendChild(messageDiv);
        });

        // Scroll to bottom
        chatMessages.scrollTop = chatMessages.scrollHeight;
    }

    escapeHtml(text) {
        const div = document.createElement('div');
        div.textContent = text;
        return div.innerHTML;
    }
}

// Initialize the app when the page loads
document.addEventListener('DOMContentLoaded', () => {
    new WebsiteApp();
});