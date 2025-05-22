# Sensor Dashboard Project

This project consists of a full-stack application for monitoring temperature data from sensors. It includes a Next.js frontend, Express.js backend, and PostgreSQL database.

## Project Structure

```
finalprojekt/
├── frontend/           # Next.js frontend application
├── backend/           # Express.js backend server
└── src.ino           # Arduino code for ESP32
```

## Prerequisites

- Node.js (v18 or later)
- PostgreSQL
- Arduino IDE (for ESP32 code)

## Setup

### Database Setup

1. Create a PostgreSQL database
2. Create a `.env` file in the backend directory with the following variables:
   ```
   DB_USER=your_db_user
   DB_PASSWORD=your_db_password
   DB_HOST=localhost
   DB_PORT=5432
   DB_NAME=your_db_name
   ```

### Backend Setup

1. Navigate to the backend directory:
   ```bash
   cd backend
   ```

2. Install dependencies:
   ```bash
   npm install
   ```

3. Start the development server:
   ```bash
   npm run dev
   ```

The backend server will run on http://localhost:3001

### Frontend Setup

1. Navigate to the frontend directory:
   ```bash
   cd frontend
   ```

2. Install dependencies:
   ```bash
   npm install
   ```

3. Start the development server:
   ```bash
   npm run dev
   ```

The frontend application will run on http://localhost:3000

## Features

- Real-time temperature monitoring
- Historical data visualization
- Responsive dashboard design
- RESTful API endpoints for data management

## API Endpoints

### Backend API

- `POST /api/sensor-data` - Receive sensor data from ESP32
- `GET /api/sensor-data` - Get current sensor readings
- `GET /api/temperature-history` - Get temperature history

### Frontend API Routes

- `/api/sensor-data` - Proxy to backend sensor data endpoint
- `/api/temperature-history` - Proxy to backend temperature history endpoint

## Development

- Frontend: Next.js with TypeScript and Tailwind CSS
- Backend: Express.js with PostgreSQL
- Charts: Recharts for data visualization
- UI Components: Custom components with shadcn/ui style

## License

MIT 