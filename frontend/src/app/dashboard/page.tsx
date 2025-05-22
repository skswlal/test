'use client';

import { useState, useEffect } from 'react';
import TemperatureChart from '@/components/TemperatureChart';
import { Card, CardContent, CardHeader, CardTitle } from '@/components/ui/card';
import { Button } from '@/components/ui/button';

export default function Dashboard() {
  const [temperature, setTemperature] = useState<number | null>(null);
  const [humidity, setHumidity] = useState<number | null>(null);
  const [loading, setLoading] = useState(true);

  useEffect(() => {
    const fetchData = async () => {
      try {
        const response = await fetch('/api/sensor-data');
        const data = await response.json();
        setTemperature(data.temperature);
        setHumidity(data.humidity);
      } catch (error) {
        console.error('Error fetching sensor data:', error);
      } finally {
        setLoading(false);
      }
    };

    fetchData();
    const interval = setInterval(fetchData, 5000); // Update every 5 seconds

    return () => clearInterval(interval);
  }, []);

  return (
    <div className="container mx-auto p-4">
      <h1 className="text-3xl font-bold mb-6">Sensor Dashboard</h1>
      
      <div className="grid grid-cols-1 md:grid-cols-2 gap-4 mb-6">
        <Card>
          <CardHeader>
            <CardTitle>Temperature</CardTitle>
          </CardHeader>
          <CardContent>
            <p className="text-4xl font-bold">
              {loading ? 'Loading...' : temperature !== null ? `${temperature}°C` : 'N/A'}
            </p>
          </CardContent>
        </Card>

        <Card>
          <CardHeader>
            <CardTitle>Humidity</CardTitle>
          </CardHeader>
          <CardContent>
            <p className="text-4xl font-bold">
              {loading ? 'Loading...' : humidity !== null ? `${humidity}%` : 'N/A'}
            </p>
          </CardContent>
        </Card>
      </div>

      <Card className="mb-6">
        <CardHeader>
          <CardTitle>Temperature History</CardTitle>
        </CardHeader>
        <CardContent>
          <TemperatureChart />
        </CardContent>
      </Card>
    </div>
  );
} 