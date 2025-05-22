const express = require('express');
const { Pool } = require('pg');
const cors = require('cors');
require('dotenv').config();

const app = express();
const port = process.env.PORT || 3001;

// Enable CORS for all routes
app.use(cors({
  origin: '*',
  methods: ['GET', 'POST', 'PUT', 'DELETE', 'OPTIONS'],
  allowedHeaders: ['Content-Type', 'Authorization']
}));

app.use(express.json());

// PostgreSQL connection
const pool = new Pool({
  user: process.env.DB_USER,
  password: process.env.DB_PASSWORD,
  host: process.env.DB_HOST,
  port: process.env.DB_PORT,
  database: process.env.DB_NAME,
  ssl: {
    rejectUnauthorized: false
  }
});

// Test database connection
pool.connect((err, client, release) => {
  if (err) {
    console.error('Error connecting to the database:', err);
  } else {
    console.log('Successfully connected to database');
    release();
  }
});

// Create tables if not exists
pool.query(`
  CREATE TABLE IF NOT EXISTS sensor_readings (
    id SERIAL PRIMARY KEY,
    timestamp TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    temperature FLOAT NOT NULL,
    humidity FLOAT
  );
`).catch(err => {
  console.error('Error creating table:', err);
});

// Health check endpoint
app.get('/api/health', (req, res) => {
  res.json({ status: 'ok', timestamp: new Date().toISOString() });
});

// Endpoint to receive sensor data from ESP32
app.post('/api/sensor-data', async (req, res) => {
  try {
    const { temperature, humidity } = req.body;
    
    // Validate input
    if (temperature === undefined) {
      return res.status(400).json({ error: 'Temperature is required' });
    }
    
    // If humidity is not provided, set it to null
    const humidityValue = humidity !== undefined ? humidity : null;
    
    const result = await pool.query(
      'INSERT INTO sensor_readings (temperature, humidity) VALUES ($1, $2) RETURNING *',
      [temperature, humidityValue]
    );
    res.json(result.rows[0]);
  } catch (error) {
    console.error('Error saving sensor data:', error);
    res.status(500).json({ 
      error: 'Internal server error',
      details: error.message 
    });
  }
});

// Endpoint to get current sensor data
app.get('/api/sensor-data', async (req, res) => {
  try {
    const result = await pool.query(
      'SELECT temperature, humidity FROM sensor_readings ORDER BY timestamp DESC LIMIT 1'
    );
    res.json(result.rows[0] || { temperature: null, humidity: null });
  } catch (error) {
    console.error('Error fetching sensor data:', error);
    res.status(500).json({ 
      error: 'Internal server error',
      details: error.message 
    });
  }
});

// Endpoint to get temperature history
app.get('/api/temperature-history', async (req, res) => {
  try {
    const result = await pool.query(
      'SELECT timestamp, temperature FROM sensor_readings ORDER BY timestamp DESC LIMIT 100'
    );
    res.json(result.rows.reverse());
  } catch (error) {
    console.error('Error fetching temperature history:', error);
    res.status(500).json({ 
      error: 'Internal server error',
      details: error.message 
    });
  }
});

// Error handling middleware
app.use((err, req, res, next) => {
  console.error(err.stack);
  res.status(500).json({ 
    error: 'Something broke!',
    details: err.message 
  });
});

app.listen(port, () => {
  console.log(`Server running on port ${port}`);
}); 