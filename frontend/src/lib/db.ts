import { Pool } from 'pg';

// PostgreSQL connection
const pool = new Pool({
  user: process.env.DB_USER,
  password: process.env.DB_PASSWORD,
  host: process.env.DB_HOST,
  port: parseInt(process.env.DB_PORT || '5432'),
  database: process.env.DB_NAME,
});

// Initialize database
export async function initDatabase() {
  try {
    await pool.query(`
      CREATE TABLE IF NOT EXISTS temperature_readings (
        id SERIAL PRIMARY KEY,
        timestamp TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
        temperature FLOAT NOT NULL
      );
    `);
    console.log('Database initialized successfully');
  } catch (error) {
    console.error('Error initializing database:', error);
    throw error;
  }
}

// Get temperature readings
export async function getTemperatureReadings(limit = 100) {
  try {
    const result = await pool.query(
      'SELECT * FROM temperature_readings ORDER BY timestamp DESC LIMIT $1',
      [limit]
    );
    return result.rows;
  } catch (error) {
    console.error('Error fetching temperatures:', error);
    throw error;
  }
}

// Add temperature reading
export async function addTemperatureReading(temperature: number) {
  try {
    const result = await pool.query(
      'INSERT INTO temperature_readings (temperature) VALUES ($1) RETURNING *',
      [temperature]
    );
    return result.rows[0];
  } catch (error) {
    console.error('Error saving temperature:', error);
    throw error;
  }
} 