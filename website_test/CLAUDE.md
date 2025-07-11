# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

"JC's Adventure" is a client-side web application with user authentication and chat functionality. Currently implemented as a static website using vanilla HTML, CSS, and JavaScript with localStorage for data persistence.

## Architecture

### Current Implementation (Static/Client-Side)
- **Frontend**: Vanilla JavaScript with class-based architecture
- **Data Storage**: Browser localStorage for users and messages
- **Authentication**: Client-side validation with hardcoded admin account
- **Chat**: Local message storage, no real-time synchronization between users

### Core Components
- `WebsiteApp` class in `script.js` - Main application controller
- Authentication system with login/register forms
- Chat interface with message display and input
- Default admin account: username "admin", password "Eypass338"

### Data Structure
- Users stored as: `{username, password, isAdmin}`
- Messages stored as: `{id, username, text, timestamp, isAdmin}`
- LocalStorage keys: `jc_adventure_users`, `jc_adventure_messages`

## Development Commands

### Local Development
```bash
# Serve files locally (any HTTP server)
python -m http.server 8000
# or
npx serve .
# or simply open index.html in browser
```

### For Backend Upgrade (Future)
When upgrading to full-stack:
- Use Node.js with Express for backend
- Implement WebSocket/Socket.io for real-time chat
- Replace localStorage with database (PostgreSQL/MongoDB)
- Add proper session-based authentication

## Key Limitations (Current State)
- No real-time chat between different browsers/users
- Data is browser-specific (localStorage)
- No server-side validation or security
- Admin credentials are hardcoded in client code

## Deployment Notes
- Current version can be deployed to any static hosting (Netlify, Vercel, GitHub Pages)
- For production use, requires backend implementation for multi-user functionality