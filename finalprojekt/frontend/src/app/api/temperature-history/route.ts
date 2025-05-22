import { NextResponse } from 'next/server';

export async function GET() {
  try {
    const response = await fetch('http://localhost:3001/api/temperature-history');
    const data = await response.json();
    
    return NextResponse.json(data);
  } catch (error) {
    console.error('Error fetching temperature history:', error);
    return NextResponse.json(
      { error: 'Failed to fetch temperature history' },
      { status: 500 }
    );
  }
} 